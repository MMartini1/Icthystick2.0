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


const char VER[] = "2.1.1"; // 2/26/2025

const unsigned long MAX_DELAY_US = 10000;
// updated 2/25/2025 M. Martini from 10 to 5
const unsigned int BLANK_TIME_US = 5;
const unsigned int LCD_LEN = 20;

const unsigned int BUTTON_MENU = 8;
const unsigned int BUTTON_NEXT = 4;
const unsigned int BUTTON_PREV = 5;
const unsigned int BUTTON_ENTER = 6;

const unsigned int LED_PIN = 0;

// set to true for feedback on menu button presses and bluetooth status
const bool DEBUG = 0;  

// serial number of box
const char IFMB[] = "001";

// output format types

const unsigned int DEFAULT_OUTPUT_FORMAT = 0;
const unsigned int ICKY_FORMAT = 0;
const unsigned int SCAN_FORMAT = 1;
const unsigned int LMNO_FORMAT = 2;
const unsigned int SERIAL_BAUD_RATE = 115200;
