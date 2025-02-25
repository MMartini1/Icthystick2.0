#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <ArduinoBLE.h>

#include "icthystick_firmware.h"
#include "lcd.h"
#include "buttons.h"
#include "menu.h"
#include "distance_sensor.h"


SoftwareSerial LCD_Serial(D6, D7, false);

BLEService newService("1821"); // creating the service
BLECharacteristic new_measurement("27A4", BLERead | BLENotify  | BLEWrite,10);

BLEByteCharacteristic switchChar("2A57", BLERead | BLEWrite); // creating the LED characteristic






typedef struct
{
  measurement_state_t measurement_state;
  button_state_t button_state;
  display_state_t display_state;
     
} system_state_t;

system_state_t system_state;

void setup() {
  // put your setup code here, to run once:

  
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, 0);

  setup_lcd();
  menu_setup(&system_state.display_state);

  if (!BLE.begin()) {

    Serial.println("starting Bluetooth® Low Energy module failed!");


    while (1);

  }


  Serial.println("Bluetooth® Low Energy Central - Peripheral Explorer");  
  BLE.setLocalName("IcthyStick");
  BLE.setAdvertisedService(newService);
  newService.addCharacteristic(new_measurement);
  BLE.addService(newService);  // adding the service
  new_measurement.writeValue("Init");
  
  BLE.advertise(); //start advertising the service

  button_setup(&system_state.button_state);
  setup_distance_sensor(&system_state.measurement_state);
  menu_system_booted();

  
}


void send_ble_measurement()
{

  
//  BLEDevice central = BLE.central(); // wait for a Bluetooth® Low Energy central
//  Serial.print("Send BLE");
//
//
//  if (central) {  // if a central is connected to the peripheral

    Serial.print("Connected to central: ");
    
    char send_str[20];
    sprintf(send_str, "%7.1f", system_state.display_state.current_measurement);
    new_measurement.writeValue(send_str);  
//  }
  
}

void pulse_led()
{
  digitalWrite(LED_PIN, 1);
  delay(100);
  digitalWrite(LED_PIN, 0);  
}


void process_measurement(long measurement_us)
{
  //float meas_in = measurement_us/9.0462;
  //float meas_mm = meas_in * 25.4;
  float meas_cm = (measurement_us - system_state.measurement_state.calibration_coefficients.b)/system_state.measurement_state.calibration_coefficients.m;
  float meas_mm = meas_cm*10;
  system_state.display_state.last_measurement = system_state.display_state.current_measurement;
  system_state.display_state.current_measurement = meas_mm;
  update_lcd_measurement_current();
  update_lcd_measurement_last();
  pulse_led();
  send_ble_measurement();
  
}

#define MEASUREMENT_VALID_DEADBAND (10)
#define MEASUREMENT_OFF_DEADBAND (5)
bool measuement_valid(uint16_t meas)
{
  return meas >=0 && meas <=system_state.measurement_state.calibration_coefficients.max_val - MEASUREMENT_VALID_DEADBAND;
}

bool measurement_off(uint16_t meas)
{
  return meas <0 || meas >=system_state.measurement_state.calibration_coefficients.max_val - MEASUREMENT_OFF_DEADBAND;
}

void run_measurement()
{
  if (millis() - system_state.measurement_state.last_read >100)
  {
    system_state.measurement_state.last_read = millis();
    long meas = make_measurement_us();
    //Serial.println(meas);
    switch (system_state.measurement_state.state)
    {
      case MEASUREMENT_STATE_WAITING:
      {
        if (measuement_valid(meas))
        {
          system_state.measurement_state.state = MEASUREMENT_STATE_MEASURED;
          process_measurement(meas);
        }
        break;
      }
      case MEASUREMENT_STATE_MEASURED:
      {
        if (measurement_off(meas))
        {
          system_state.measurement_state.state = MEASUREMENT_STATE_WAITING;
        }
        break;
      }

      case MEASUREMENT_STATE_CAL_MAX_VAL:
      {
        system_state.measurement_state.calibration_coefficients.max_val = meas;
        system_state.measurement_state.state = MEASUREMENT_STATE_CAL1_WAITING;
        
        break;
      }
      case   MEASUREMENT_STATE_CAL1_WAITING:
      {
        if (measuement_valid(meas))
        {
          // save cal parameter
          Serial.println("CAL1");
          system_state.measurement_state.pending_calibration.measured[system_state.measurement_state.pending_calibration.next_cal_point] = meas;
          system_state.measurement_state.state = MEASUREMENT_STATE_CAL1_MEASURED;
          system_state.measurement_state.pending_calibration.next_cal_point+=1;
          refresh_calibration_view();
        }
        break;
      } 
      case   MEASUREMENT_STATE_CAL1_MEASURED:
      {
        if (measurement_off(meas))
        {
          system_state.measurement_state.state = MEASUREMENT_STATE_CAL2_WAITING;
        }
        break;
      }       
      case   MEASUREMENT_STATE_CAL2_WAITING:
      {
        if (measuement_valid(meas))
        {
          // save cal parameter
          system_state.measurement_state.pending_calibration.measured[system_state.measurement_state.pending_calibration.next_cal_point] = meas;
          
          system_state.measurement_state.state = MEASUREMENT_STATE_CAL2_MEASURED;
          system_state.measurement_state.pending_calibration.next_cal_point+=1;
          
          refresh_calibration_view();
          
        }
        break;
      } 
      case   MEASUREMENT_STATE_CAL2_MEASURED:
      {
        if (measurement_off(meas))
        {
          system_state.measurement_state.state = MEASUREMENT_STATE_CAL3_WAITING;
        }
        break;
      }  
      case   MEASUREMENT_STATE_CAL3_WAITING:
      {
        if (measuement_valid(meas))
        {
          // save cal parameter
          system_state.measurement_state.pending_calibration.measured[system_state.measurement_state.pending_calibration.next_cal_point] = meas;
          
          system_state.measurement_state.state = MEASUREMENT_STATE_CAL3_MEASURED;
          system_state.measurement_state.pending_calibration.next_cal_point+=1;          
          refresh_calibration_view();
        }
        break;
      } 
      case   MEASUREMENT_STATE_CAL3_MEASURED:
      {
        if (measurement_off(meas))
        {
          system_state.measurement_state.state = MEASUREMENT_STATE_CAL4_WAITING;
        }
        break;
      }   
      case   MEASUREMENT_STATE_CAL4_WAITING:
      {
        if (measuement_valid(meas))
        {
          // save cal parameter
          // return to menu
          system_state.measurement_state.pending_calibration.measured[system_state.measurement_state.pending_calibration.next_cal_point] = meas;

          calculate_calibration();
          system_state.measurement_state.state = MEASUREMENT_STATE_MEASURED;
          system_state.measurement_state.pending_calibration.next_cal_point+=1;
          system_state.display_state.active_menu = &system_state.display_state.calibration_results_menu;
          system_state.display_state.active_menu->redraw();
        }
        break;
      } 
    }    
  }  
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEDevice central = BLE.central(); // need to call this in the loop for the bluetooth state machine
  
  read_buttons();
  run_measurement();


  
}
