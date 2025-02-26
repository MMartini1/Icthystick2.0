void setup_lcd()
{
  LCD_Serial.begin(19200);
  clear_lcd();
  backlight_off();    
}

void clear_lcd()
{
  
  LCD_Serial.write(0xC);  
  delay(5);
}

void backlight_on()
{
  LCD_Serial.write(17);  
}

void backlight_off()
{
  LCD_Serial.write(18);  
}

void move_cursor(char line, char pos)
{
  unsigned char c = 0x80;
  c+= line*20;
  c+= pos;
  LCD_Serial.write(c);  
}

// MM 2/26/2025 so we can beep
void beep()
{
  LCD_Serial.write(212);  // 1/4 note
  LCD_Serial.write(227);  // play E
}
