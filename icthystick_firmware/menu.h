
#ifndef MENU_H
#define MENU_H




typedef   void (*redraw_fcn_t)(void);
typedef   void (*enter_fcn_t)(void);
typedef   void (*next_fcn_t)(void);
typedef   void (*prev_fcn_t)(void);
typedef   void (*menu_fcn_t)(void);

typedef struct 
{
  redraw_fcn_t redraw;
  enter_fcn_t enter;
  next_fcn_t next;
  prev_fcn_t prev;
  menu_fcn_t menu;
  uint8_t options;
  uint8_t selected_option;
  uint8_t option_rows[4]; // row to set cursor to when the option is selected
  uint8_t option_cols[4]; // column to set cursor to when the option is selected
} menu_commands;





typedef enum
{
  DISPLAY_STATE_MAIN=0,
  DISPLAY_STATE_COMMUNICATIONS_MENU,
  DISPLAY_STATE_DISPLAY_MENU,
  DISPLAY_STATE_SOUND_MENU,
  DISPLAY_STATE_MEASUREMENT_MENU,
  DISPLAY_STATE_FORMAT_MENU,
  DISPLAY_STATE_CALIBRATION_MENU,
  DISPLAY_STATE_SAVE_CONFIG_MENU
} e_display_states;


typedef enum
{
  UNIT_MM=0,
  UNIT_CM
} e_system_units;

typedef enum
{
  PRECISION_DEC_0,
  PRECISION_DEC_1
} e_measurement_precision;

typedef enum 
{
  ICKY_FORMAT=0,
  SCAN_FORMAT,
  LMNO_FORMAT,
  CFF_FORMAT

} e_output_format;

typedef struct
{
  float last_measurement;
  float current_measurement;
  e_system_units display_units;
  e_output_format output_format_type = ICKY_FORMAT;
  e_measurement_precision display_precision;
  bool display_backlight_on;
  bool sound_on;
  //unsigned int output_format_state;
  e_display_states display_state;
  menu_commands* active_menu;
  menu_commands main_menu;
  menu_commands display_menu;
  menu_commands sound_menu;
  menu_commands measurement_menu;
  menu_commands format_menu;
  menu_commands calibration_menu;
  menu_commands calibration_start_menu;
  menu_commands calibration_results_menu;
  menu_commands save_menu;
  menu_commands save_menu_results;
  menu_commands calibration_run_menu;
} display_state_t;


void menu_setup(display_state_t* ds);
void refresh_calibration_view();
void menu_system_booted();


#endif
