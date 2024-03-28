
#ifndef BUTTON_H
#define BUTTON_H

typedef struct
{
  bool button_menu_pressed;
  bool button_next_pressed;
  bool button_prev_pressed;
  bool button_enter_pressed;
  bool button_menu_released;
  bool button_next_released;
  bool button_prev_released;
  bool button_enter_released; 
  unsigned long last_read; 
} button_state_t;


void button_setup(button_state_t* button_state);
void read_buttons();

#endif
