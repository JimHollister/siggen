/*
* Functions for handling the LCD display.
*/

#ifndef LCD_H_
#define LCD_H_

// LCD symbols. OR these together to get multiple symbols
#define LCD_SYM_NONE 0x00			// all symbols off
#define LCD_SYM_COLON_RIGHT 0x08	// colon in right-half LCD display
#define LCD_SYM_DP2 0x04			// decimal point before digit 2
#define LCD_SYM_DP3 0x02			// decimal point before digit 3
#define LCD_SYM_DP4 0x01			// decimal point before digit 4
#define LCD_SYM_COLON_LEFT 0x80		// colon in left-half LCD display
#define LCD_SYM_DP6 0x40			// decimal point before digit 6
#define LCD_SYM_DP7 0x20			// decimal point before digit 7
#define LCD_SYM_DP8 0x10			// decimal point before digit 8
#define LCD_SYM_ALL 0xFF			// all symbols on

void lcd_initialize();

void lcd_show_ascii(char* eight_char_buf);
void lcd_show_ascii_with_symbols(char* eight_char_buf, uint8_t symbols);
void lcd_show_integer(uint32_t value);
void lcd_show_integer_with_symbols(uint32_t value, uint8_t symbols);
void lcd_clear();
void lcd_segment_test();

void lcd_test_ordered();
void lcd_test_off();
void lcd_test_on();

#endif /* LCD_H_ */