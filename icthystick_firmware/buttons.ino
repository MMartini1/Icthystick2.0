
#include "buttons.h"

button_state_t* button_state;

void button_setup(button_state_t* bs)
{
  button_state = bs;
  pinMode(BUTTON_MENU, INPUT_PULLUP);
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_PREV, INPUT_PULLUP);
  pinMode(BUTTON_ENTER, INPUT_PULLUP);   

  button_state->button_menu_pressed=false;
  button_state->button_next_pressed=false;
  button_state->button_prev_pressed=false;
  button_state->button_enter_pressed=false;
  button_state->button_menu_released=true;
  button_state->button_next_released=true;
  button_state->button_prev_released=true;
  button_state->button_enter_released=true;
  button_state->last_read = millis();
  
}

typedef enum
{
  BTN_CMD_MENU = 0,
  BTN_CMD_NEXT,
  BTN_CMD_PREV,
  BTN_CMD_ENTER
} e_button_commands;



void read_buttons()
{
  if (millis() - button_state->last_read <20)
  {
    return; // only process every so often to avoid bouncing  
  }
  button_state->last_read = millis();
    
  int buttonVal = digitalRead(BUTTON_MENU);
  if (!buttonVal && button_state->button_menu_released)
  {
    if (!button_state->button_menu_pressed)
    {
      button_state->button_menu_released = false;
      button_state->button_menu_pressed = true;
      if (DEBUG) Serial.println("MENU");

      handle_menu_button();
    }
  }
  else if (buttonVal && !button_state->button_menu_released)
  {
    if (button_state->button_menu_pressed)
      {
        button_state->button_menu_released = true;
        button_state->button_menu_pressed = false;    
        if (DEBUG) Serial.println("menu released");
      }
  }
  buttonVal = digitalRead(BUTTON_ENTER);
  if (!buttonVal && button_state->button_enter_released)
  {
    if (!button_state->button_enter_pressed)
    {
      button_state->button_enter_released = false;
      button_state->button_enter_pressed = true;
      if (DEBUG) Serial.println("ENTER");

      handle_enter_button();
    }
  }
  else if (buttonVal && !button_state->button_enter_released)
  {
    if (button_state->button_enter_pressed)
      {
        button_state->button_enter_released = true;
        button_state->button_enter_pressed = false;    
        if (DEBUG) Serial.println("enter released");
      }
  }  

  buttonVal = digitalRead(BUTTON_NEXT);
  if (!buttonVal && button_state->button_next_released)
  {
    if (!button_state->button_next_pressed)
    {
      button_state->button_next_released = false;
      button_state->button_next_pressed = true;
      if (DEBUG) Serial.println("NEXT");

      handle_next_button();
    }
  }
  else if (buttonVal && !button_state->button_next_released)
  {
    if (button_state->button_next_pressed)
      {
        button_state->button_next_released = true;
        button_state->button_next_pressed = false;    
        if (DEBUG) Serial.println("next released");
      }
  }  

  buttonVal = digitalRead(BUTTON_PREV);
  if (!buttonVal && button_state->button_prev_released)
  {
    if (!button_state->button_prev_pressed)
    {
      button_state->button_prev_released = false;
      button_state->button_prev_pressed = true;
      if (DEBUG) Serial.println("PREV");

      handle_prev_button();
    }
  }
  else if (buttonVal && !button_state->button_prev_released)
  {
    if (button_state->button_prev_pressed)
      {
        button_state->button_prev_released = true;
        button_state->button_prev_pressed = false;    
        if (DEBUG) Serial.println("prev released");
      }
  }  
  

  //print out the value of the pushbutton

  //Serial.println(buttonVal);
}
