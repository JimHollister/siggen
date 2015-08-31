/*
* Functions for handling the LCD display.
*/

#ifndef LCD_H_
#define LCD_H_

void lcd_initialize();
void lcd_send_variable_bits(uint8_t value, uint8_t num_bits);
void lcd_update_display();
void lcd_test_ordered();
void lcd_test_off();
void lcd_test_on();

#endif /* LCD_H_ */