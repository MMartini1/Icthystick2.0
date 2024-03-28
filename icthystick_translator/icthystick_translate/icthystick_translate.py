import asyncio
import serial
import bleak

import argparse

s=None

class Icthystick:
    def __init__(self, port):
        print("Opening port")
        self.serial=serial.Serial(port, baudrate=9600)
        self.disconnected = False

    def disconnect_callback(self):
        self.disconnected = True

    async def find_icthystick(self, device_mask):
        devices = await bleak.BleakScanner.discover(
            return_adv=True
        )
        device = None
        for d, a in devices.values():
            if d.name == "IcthyStick" and d.address.startswith(device_mask):
                device = d
                break;
        return device
        #device = await bleak.BleakScanner.find_device_by_name("IcthyStick")


        return device

    async def find_char(self, client):
        for s in client.services:
            for char in s.characteristics:
                if "read" in char.properties:
                    try:
                        value = await client.read_gatt_char(char.uuid)
                        break;
                    except Exception as e:
                        print(e)
                        return None
        return value


    def notification_handle(self, charecteristic: bleak.backends.characteristic.BleakGATTCharacteristic, data: bytearray):
        print(data)
        st = data.decode()
        print(st)
        try:
            length_mm = float(st)
            send_str = b"$IFMB,%07.1f,mm \r\n"%length_mm
            self.serial.write(send_str)
            self.serial.flushOutput()
            print(send_str)
        except ValueError:
            pass # ignore the init message

    async def register_notification(self, client):
        for s in client.services:
            for char in s.characteristics:
                print(char.properties)
                if "notify" in char.properties:
                    print(char)
                    try:
                        await client.start_notify(char, self.notification_handle)
                        print("Done register")
                    except Exception as e:
                        print(e)



async def main_async(ic, device_mask):
    print("Searching for Bluetooth")
    device = await ic.find_icthystick(device_mask)
    print("Device Found: " + repr(device))
    if device is None:
        return None
    
    services = ["1821"]
    async with bleak.BleakClient(device) as client:
        await ic.register_notification(client)
    
        while True:
            if not client.is_connected:
                print("disconnected")
                break;
            await asyncio.sleep(1)


def main():

    parser = argparse.ArgumentParser(description='Process some integers.')
    parser.add_argument('--com_port', dest='com_port', type=str, nargs='?', default="COM101",
                        help='com_port to use for communicating with database')
    parser.add_argument('--device_mask', dest='device_mask', 
                        default="",
                        help='mask to match bluetooth device')

    args = parser.parse_args()
    
    ic = Icthystick(args.com_port)
    while True:
        asyncio.run(main_async(ic, device_mask=args.device_mask))
        

if __name__ == "__main__":
    main()        
              
