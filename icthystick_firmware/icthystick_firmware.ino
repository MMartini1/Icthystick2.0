#include <SoftwareSerial.h> // https://docs.arduino.cc/learn/built-in-libraries/software-serial/
#include <EEPROM.h>
#include <ArduinoBLE.h>

#include "icthystick_firmware.h"
#include "lcd.h"
#include "buttons.h"
#include "menu.h"
#include "distance_sensor.h"


SoftwareSerial LCD_Serial(D6, D7, false);

BLEService newService("1821"); // creating the service
BLECharacteristic new_measurement("27A4", BLERead | BLENotify  | BLEWrite, MAX_BLE_VALUESIZE); // was 10

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


  if (DEBUG) Serial.println("Bluetooth® Low Energy Central - Peripheral Explorer");  
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

    if (DEBUG) Serial.print("Connected to central: ");
    
    char send_str[MAX_BLE_VALUESIZE]; // increased from 20 to accommodate scantrol format
    float m;

    // current_measurement is in mm
    switch (system_state.display_state.output_format_type){
      case ICKY_FORMAT:  // in mm or cm?
        if(system_state.display_state.display_units == UNIT_MM) {    
          sprintf(send_str, "$IFMB%s,%05.1f,mm\n", IFMB, system_state.display_state.current_measurement);
        } else {
          sprintf(send_str, "$IFMB%s,%05.1f,cm\n", IFMB, system_state.display_state.current_measurement/10.0);
        }
        break;
      case SCAN_FORMAT:  // in cm
        if(system_state.display_state.display_units == UNIT_MM) 
          m = system_state.display_state.current_measurement / 10.0;
        sprintf(send_str, "0001 001 LENGTH %019.1f 001 #8B\n", m);
        break;
      case LMNO_FORMAT:  // in mm
        sprintf(send_str, "%05.1frr\n", system_state.display_state.current_measurement);
        break;
      case CFF_FORMAT:  // CFF format
        sprintf(send_str, "%7.1f\n", system_state.display_state.current_measurement);
        break;
        
    }
    if (sizeof(send_str) <= MAX_BLE_VALUESIZE) new_measurement.writeValue(send_str);  // this goes to Bluetooth
    // MM 2/26/2025 add this line to see measurement on serial console
    Serial.println(send_str);
//  }
  
}

void pulse_led()
{
  digitalWrite(LED_PIN, 1);
  delay(200);  // MM 2/26/2025 increase on time of LED
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
  if(system_state.display_state.sound_on){
    beep();    // MM 2/26/2025 need sound too
  }
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
