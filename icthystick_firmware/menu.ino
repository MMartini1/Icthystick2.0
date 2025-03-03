#include "distance_sensor.h"

static display_state_t* display_state;

void menu_setup(display_state_t* ds)
{
  draw_splash_screen();
  display_state = ds;
  display_state->active_menu = &display_state->main_menu;
  display_state->main_menu.redraw = draw_lcd_main_background;
  display_state->main_menu.enter = 0;
  display_state->main_menu.next = 0;
  display_state->main_menu.prev = 0;
  display_state->main_menu.menu = switch_menu_to_backlight;
  display_state->main_menu.options = 0;
  display_state->main_menu.selected_option = 0;

  display_state->display_menu.redraw = draw_lcd_display_background;
  display_state->display_menu.enter = toggle_lcd_backlight;
  display_state->display_menu.next = 0;
  display_state->display_menu.prev = 0;
  display_state->display_menu.menu = switch_menu_to_sound;  // MM 2/26/2025 changed from switch_menu_to_measurement
  display_state->display_menu.options = 1;
  display_state->display_menu.selected_option = 0;
  display_state->display_menu.option_rows[0] = 3;
  display_state->display_menu.option_cols[0] = 15; 

  display_state->sound_menu.redraw = draw_lcd_sound_background;
  display_state->sound_menu.enter = toggle_lcd_sound;
  display_state->sound_menu.next = 0;
  display_state->sound_menu.prev = 0;
  display_state->sound_menu.menu = switch_menu_to_measurement;
  display_state->sound_menu.options = 1;
  display_state->sound_menu.selected_option = 0;
  display_state->sound_menu.option_rows[0] = 3;
  display_state->sound_menu.option_cols[0] = 15; 

  display_state->measurement_menu.redraw = draw_lcd_measurement_background;
  display_state->measurement_menu.enter = measurement_enter_callback;
  display_state->measurement_menu.next = next_item_callback;
  display_state->measurement_menu.prev = prev_item_callback;
  display_state->measurement_menu.menu = switch_menu_to_format;
  display_state->measurement_menu.options = 2;
  display_state->measurement_menu.selected_option = 0;
  display_state->measurement_menu.option_rows[0] = 2;
  display_state->measurement_menu.option_cols[0] = 8;   
  display_state->measurement_menu.option_rows[1] = 3;
  display_state->measurement_menu.option_cols[1] = 8;   
  
  display_state->format_menu.redraw = draw_lcd_format_background;
  display_state->format_menu.enter = toggle_format_type;
  display_state->format_menu.next = 0;
  display_state->format_menu.prev = 0;
  display_state->format_menu.menu = switch_menu_to_calibration;
  display_state->format_menu.options = 1;
  display_state->format_menu.selected_option = 0;
  display_state->format_menu.option_rows[0] = 3;
  display_state->format_menu.option_cols[0] = 15; 

  display_state->calibration_menu.redraw = draw_lcd_calibration_background;
  display_state->calibration_menu.enter = calibration_enter_callback;
  display_state->calibration_menu.next = 0;
  display_state->calibration_menu.prev = 0;
  display_state->calibration_menu.menu = switch_menu_to_save_config;
  display_state->calibration_menu.options = 0;
  display_state->calibration_menu.selected_option = 0;

  display_state->calibration_start_menu.redraw = draw_lcd_calibration_start_background;
  display_state->calibration_start_menu.enter = calibration_start_enter_callback;
  display_state->calibration_start_menu.next = 0;
  display_state->calibration_start_menu.prev = 0;
  display_state->calibration_start_menu.menu = abort_calibration_callback; 
  display_state->calibration_start_menu.options = 0;
  display_state->calibration_start_menu.selected_option = 0;
  
  display_state->calibration_run_menu.redraw = draw_lcd_calibration_run_background;
  display_state->calibration_run_menu.enter = 0;
  display_state->calibration_run_menu.next = 0;
  display_state->calibration_run_menu.prev = 0;
  display_state->calibration_run_menu.menu = abort_calibration_callback; 
  display_state->calibration_run_menu.options = 0;
  display_state->calibration_run_menu.selected_option = 0;

  display_state->calibration_results_menu.redraw = draw_lcd_calibration_results_background;
  display_state->calibration_results_menu.enter = 0;
  display_state->calibration_results_menu.next = 0;
  display_state->calibration_results_menu.prev = 0;
  display_state->calibration_results_menu.menu = abort_calibration_callback;
  display_state->calibration_results_menu.options = 0;
  display_state->calibration_results_menu.selected_option = 0;

  display_state->save_menu.redraw = draw_lcd_save_config_background;
  display_state->save_menu.enter = handle_save_config_option;
  display_state->save_menu.next = 0;
  display_state->save_menu.prev = 0;
  display_state->save_menu.menu = switch_menu_to_main;  
  display_state->save_menu.options = 0;
  display_state->save_menu.selected_option = 0;

  display_state->save_menu_results.redraw = draw_lcd_save_results_background;
  display_state->save_menu_results.enter = switch_menu_to_main;
  display_state->save_menu_results.next = 0;
  display_state->save_menu_results.prev = 0;
  display_state->save_menu_results.menu = switch_menu_to_main;  
  display_state->save_menu_results.options = 0;
  display_state->save_menu_results.selected_option = 0;


  display_state->last_measurement = 0;
  display_state->current_measurement;
  display_state->display_units = UNIT_MM;
  display_state->display_precision = PRECISION_DEC_1;
  display_state->display_backlight_on= false;
  display_state->sound_on=false;
  display_state->display_state= DISPLAY_STATE_MAIN;
  display_state->output_format_type=ICKY_FORMAT;
  

//  draw_lcd_main_background();
//  display_state->active_menu->redraw();    
}

void menu_system_booted()
{
  switch_menu_to_main();  
}

void handle_save_config_option()
{
  store_calibration_coefficients();
  display_state->active_menu = &display_state->save_menu_results;
  display_state->active_menu->redraw();
  
}


void handle_menu_button()
{
  if (display_state->active_menu->menu != 0)
  {
    display_state->active_menu->menu();
  }
  update_cursor_pos(); 
  
}

void handle_enter_button()
{
  if (display_state->active_menu->enter !=0)
  {
    display_state->active_menu->enter();  
  }
  update_cursor_pos(); 

}


void handle_next_button()
{
  if (display_state->active_menu->next !=0)
  {
    display_state->active_menu->next();  
  }
  update_cursor_pos(); 

}


void handle_prev_button()
{
  if (display_state->active_menu->prev !=0)
  {
    display_state->active_menu->prev();  
  }
  update_cursor_pos(); 

}

void draw_splash_screen()
{
  move_cursor(0, 0);
  LCD_Serial.write("IFMB S/N ");   
  LCD_Serial.write(IFMB);   
  move_cursor(1, 0);
  LCD_Serial.write("Icthystick V");
  LCD_Serial.write(VER);   
  move_cursor(2, 0);
  LCD_Serial.write("Booting...");   
  
}

void update_lcd_measurement_last()
{
  if (display_state->active_menu != &display_state->main_menu)
  {
    Serial.println("Can't update measurement when menu not active");
    return; // only update if the proper menu is active  
  }
  char str[20];
  float meas = display_state->last_measurement;
  if (display_state->display_units == UNIT_CM)
  {
    meas = meas/10;
  }
  sprintf(str, "%9.1f", meas);
  move_cursor(2, 8);
  LCD_Serial.write(str); 
  
}

void update_lcd_measurement_current()
{
  char str[20];
  float meas = display_state->current_measurement;
  if (display_state->display_units == UNIT_CM)
  {
    meas = meas/10;  
  }
  sprintf(str, "%9.1f", meas);
  move_cursor(3, 8);
  LCD_Serial.write(str);   
}

void update_lcd_measurement_units()
{
  move_cursor(3, LCD_LEN-3);
  
  if (display_state->display_units == UNIT_MM){
    LCD_Serial.write("mm"); 
  }
  else if (display_state->display_units == UNIT_CM){
    LCD_Serial.write("cm"); 
  }  
}

void draw_lcd_main_background()
{
  clear_lcd();
  move_cursor(0,0);
  LCD_Serial.write("--->  MEASURING <---");
  move_cursor(3,0);
  LCD_Serial.write("LENGTH=");
  update_lcd_measurement_units();
  update_lcd_measurement_current();
  update_lcd_measurement_last();
 
}

void update_lcd_display_backlight()
{
  move_cursor(3, 15);
  if(DEBUG) Serial.println("Update backlight");
  Serial.println(display_state->display_backlight_on);
  if (display_state->display_backlight_on){
    LCD_Serial.write(" ON"); 
  }
  else{
    LCD_Serial.write("OFF"); 
  }  
}

void update_lcd_sound_status()
{
  move_cursor(3, 8);
  
  if (display_state->sound_on){
    LCD_Serial.write(" ON"); 
  }
  else{
    LCD_Serial.write("OFF"); 
  }    
}

void update_measurement_menu_units()
{
  move_cursor(2,8);
  if (display_state->display_units == UNIT_MM)
  {
    LCD_Serial.write("MM");  
  }  
  else
  {
    LCD_Serial.write("CM");  
  }
}

void update_format_menu_format()
{
  move_cursor(2,8);
  switch(display_state->output_format_type){
    case ICKY_FORMAT:
      LCD_Serial.write("Icthystick");
      break;
    case SCAN_FORMAT:
      LCD_Serial.write("Scantrol  ");
      break;
    case LMNO_FORMAT:
      LCD_Serial.write("Limno     ");
      break;
    case CFF_FORMAT:
      LCD_Serial.write("CFF       ");
      break;
  }
}

void update_measurement_menu_precision()
{
  move_cursor(3,8);
  if (display_state->display_precision == PRECISION_DEC_1)
  {
    LCD_Serial.write("XXXX.X");  
  }  
  else
  {
    LCD_Serial.write("XXXXX");  
  }
}

void update_cursor_pos()
{
  if (display_state->active_menu->options ==0)
  {
    move_cursor(3,20);
  }
  else
  {
    move_cursor(display_state->active_menu->option_rows[display_state->active_menu->selected_option],
                display_state->active_menu->option_cols[display_state->active_menu->selected_option]);  
  }
}

void draw_lcd_display_background()
{
  clear_lcd();
  move_cursor(0,0);
  LCD_Serial.write("Display Menu:");
  move_cursor(2,0);
  LCD_Serial.write(" Backlighting:");
  update_lcd_display_backlight(); 
  update_cursor_pos(); 
}

void draw_lcd_sound_background()
{
  clear_lcd();
  move_cursor(0,0);
  LCD_Serial.write("Sound Menu:");
  move_cursor(2,0);
  LCD_Serial.write(" Sound:");
  update_lcd_sound_status(); 
  update_cursor_pos(); 

}

void draw_lcd_calibration_background()
{
  clear_lcd();
  move_cursor(0,0);
  LCD_Serial.write("Calibration Menu:");
  move_cursor(2,0);
  LCD_Serial.write("Press ENT Begin Cal");
  move_cursor(3,0);
  LCD_Serial.write("Press MNU to Quit");
  update_cursor_pos(); 
  
}

void refresh_calibration_view()
{
  draw_lcd_calibration_run_background();  
}

void draw_lcd_calibration_start_background()
{
  clear_lcd();
  move_cursor(0,0);
  LCD_Serial.write("PREPARE CAL");
  move_cursor(1,0);
  LCD_Serial.write("Remove Wand");
  move_cursor(2,0);
  
  LCD_Serial.write("Press ENT to begin");
  move_cursor(3,0);
  
  LCD_Serial.write("Press MNU to Quit");
  update_cursor_pos();       
}

void draw_lcd_calibration_run_background()
{
  clear_lcd();
  move_cursor(0,0);
  LCD_Serial.write("Calibrating...");
  move_cursor(1,0);
  LCD_Serial.write("Target Point is:");
  move_cursor(2,0);
  char str[20];
  sprintf(str, "%9.1fcm", get_current_calibration_target());
  LCD_Serial.write(str);
  move_cursor(3,0);
  LCD_Serial.write("Press MNU to Quit");
  update_cursor_pos();   
}

void draw_lcd_calibration_results_background()
{
  clear_lcd();
  move_cursor(0,0);
  LCD_Serial.write("Calibration Results...");
  move_cursor(1,0);
  char str[20];
  sprintf(str, "m = %9.1f us/cm", get_current_calibration_slope());  
  LCD_Serial.write(str);
  move_cursor(2,0);
  sprintf(str, "b = %9.1f us", get_current_calibration_intercept());  
  LCD_Serial.write(str);
  move_cursor(3,0);
  LCD_Serial.write("Press MNU to Quit");
  update_cursor_pos();   
}

void draw_lcd_measurement_background()
{
  clear_lcd();
  move_cursor(0,0);
  LCD_Serial.write("Measurement Menu:");
  move_cursor(2,0);
  LCD_Serial.write(" UNITS:");
  move_cursor(3,0);
  LCD_Serial.write(" DEC:");
  update_measurement_menu_units();
  update_measurement_menu_precision();
  update_cursor_pos(); 
  

}

void draw_lcd_format_background()
{
  clear_lcd();
  move_cursor(0,0);
  LCD_Serial.write("Format Menu:");
  move_cursor(2,0);
  LCD_Serial.write(" FORMAT:");
  update_format_menu_format();
  update_cursor_pos(); 
  

}

void draw_lcd_save_config_background()
{
  clear_lcd();
  move_cursor(0,0);
  LCD_Serial.write("Save Config Menu:");
  move_cursor(2,0);
  LCD_Serial.write("Press ENT to Save");
  move_cursor(3,0);
  LCD_Serial.write("Press MNU to Quit");
  update_cursor_pos(); 
  
}

void draw_lcd_save_results_background()
{
  clear_lcd();
  move_cursor(0,0);
  LCD_Serial.write("Calibration Saved");
  move_cursor(2,0);
  LCD_Serial.write("Press ENT or MNU");
  update_cursor_pos();   
}


void switch_menu_to_backlight()
{
  if (DEBUG) Serial.println("Switch to Display");

  display_state->active_menu = &display_state->display_menu;
  display_state->active_menu->redraw();
}

void switch_menu_to_main()
{
  if (DEBUG) Serial.println("Switch to Main");
  display_state->active_menu = &display_state->main_menu;
  display_state->active_menu->redraw();
}

void switch_menu_to_measurement()
{
  display_state->active_menu = &display_state->measurement_menu;
  display_state->active_menu->redraw();  
}

// MM 2/26/2025
void switch_menu_to_sound()
{
  display_state->active_menu = &display_state->sound_menu;
  display_state->active_menu->redraw();  
}

void switch_menu_to_format()
{
  display_state->active_menu = &display_state->format_menu;
  display_state->active_menu->redraw();  
}

void switch_menu_to_calibration()
{
  display_state->active_menu = &display_state->calibration_menu;
  display_state->active_menu->redraw();  
}

void switch_menu_to_save_config()
{
  display_state->active_menu = &display_state->save_menu;
  display_state->active_menu->redraw();  
}

void abort_calibration_callback()
{
  display_state->active_menu = &display_state->calibration_menu;
  abort_calibration();
  display_state->active_menu->redraw();  
  
}

void toggle_lcd_backlight()
{
  Serial.println("Toggle");
  display_state->display_backlight_on=!display_state->display_backlight_on;
  update_lcd_display_backlight();  
  if (display_state->display_backlight_on)
  {
    backlight_on();
  }
  else
  {
    backlight_off();
  }
}

// MM 2/26/2025
void toggle_lcd_sound()
{
  Serial.println("Toggle sound");
  display_state->sound_on=!display_state->sound_on;
  update_lcd_sound_status();  
}

void toggle_measurement_units()
{
  if (display_state->display_units == UNIT_MM)
  {
    display_state->display_units = UNIT_CM;  
  }  
  else
  {
    display_state->display_units = UNIT_MM;  
  }
  update_measurement_menu_units();
}

void toggle_measurement_precision()
{
  if (display_state->display_precision == PRECISION_DEC_1)
  {
    display_state->display_precision = PRECISION_DEC_0;  
  }  
  else
  {
    display_state->display_precision = PRECISION_DEC_1;  
  }
  update_measurement_menu_precision();
}

void toggle_format_type()
{
  if (display_state->output_format_type == ICKY_FORMAT)
  {
    display_state->output_format_type = SCAN_FORMAT;  
  }  
  else if (display_state->output_format_type == SCAN_FORMAT)
  {
    display_state->output_format_type = LMNO_FORMAT;   
  }
  else if (display_state->output_format_type == LMNO_FORMAT)
  {
    display_state->output_format_type = CFF_FORMAT;   
  }
  else if (display_state->output_format_type == CFF_FORMAT)
  {
    display_state->output_format_type = ICKY_FORMAT;   
  }
  update_format_menu_format();
}

void next_item_callback()
{
  if (display_state->active_menu->options > 0)
  {
    display_state->active_menu->selected_option = (display_state->active_menu->selected_option + 1)%display_state->active_menu->options;
    update_cursor_pos(); 
    
  }  
  
}


void prev_item_callback()
{
  if (display_state->active_menu->options > 0)
  {
    display_state->active_menu->selected_option = ((uint8_t) (display_state->active_menu->selected_option - 1))%display_state->active_menu->options;
    update_cursor_pos();     
  }  
  
}

void measurement_enter_callback()
{
  if (display_state->active_menu->selected_option == 0)
  {
    toggle_measurement_units();
  }
  else
  {
    toggle_measurement_precision();  
  }
}

void calibration_enter_callback()
{
  display_state->active_menu = &display_state->calibration_start_menu;
  display_state->active_menu->redraw();    

}

void calibration_start_enter_callback()
{
  initiate_calibration();
  display_state->active_menu = &display_state->calibration_run_menu;
  display_state->active_menu->redraw();  
  Serial.println("enter calibration run");
  
  // update measurement state to wait for cal tap
  // update screen to show cal target.
  // watch for menu to cancel abort_calibration()
  // once done, wait for confirmation?
  //   
}
