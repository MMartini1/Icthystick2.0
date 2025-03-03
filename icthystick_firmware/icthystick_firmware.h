// Arduino code for AcBotics drop in IFMB display electronics
// original code by AcBotics https://github.com/Acbotics-Public/Icthystick2.0
//
// Updated by M. Martini to accommodate NOAA NEFSC needs
// https://github.com/MMartini1/Icthystick2.0
// - 2.1.0 
//   add beep and menu optino to turn off beep
// - 2.1.1
//   add output formats
//   print only measurement to serial output unless in debug mode
// - 2.2.0
//   update manu system to select output formats

// user defines
const char IFMB[] = "000";  // serial number of box
const char VER[] = "2.2.0"; // 3/3/2025
// to change the default output format
// in menu.h change e_output_format output_format_type = ICKY_FORMAT;
// set to true for feedback on menu button presses and bluetooth status
const bool DEBUG = 1;  

// other defines
const unsigned long MAX_DELAY_US = 10000;
const unsigned int MAX_BLE_VALUESIZE = 80;

// updated 2/25/2025 M. Martini from 10 to 5
const unsigned int BLANK_TIME_US = 5;
const unsigned int LCD_LEN = 20;

const unsigned int BUTTON_MENU = 8;
const unsigned int BUTTON_NEXT = 4;
const unsigned int BUTTON_PREV = 5;
const unsigned int BUTTON_ENTER = 6;

const unsigned int LED_PIN = 0;

