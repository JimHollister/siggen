/*
* Functions for handling the LCD display.
*
* The timing of operations in this function must meet the minimum timing constraints of the
* Microchip 80438 display driver. This functions assumes a system clock of 16.384 MHz and that this
* function has been compiled with optimization level -O1 or -Os). If the function is not compiled
* with optimization (-O0) the function will still work, but will send bits to the LCD more slowly.
*/

#include <avr/io.h>
#include "lcd.h"
#include "bcd.h"

//////////////////////////////////////////////////////////////////////////
// Private variables and functions
//////////////////////////////////////////////////////////////////////////

/* 
* Send the specified number of bits to the LCD shift register. 1 <= num_bits <= 8.
* It's assumed that the bits are right-aligned in the value byte. For example, to send the 4-bit
* value 0b1010 to the LCD: value = 0x0A and num_bits = 4.
*/
void lcd_send_variable_bits(uint8_t value, uint8_t num_bits)
{
	// Left align the value bits because the USI shift register sends the high bits first.
	for (uint8_t i=num_bits; i<8; i++) {
		value <<= 1;
	}

	PORTB |= _BV(PORTB7);					// Set USCK high

	// Send the shifted value. Top bit goes first.
	USIDR = value;							// Load byte to be sent. This sets the DO pin to the value of the top bit.
	for (uint8_t i=0; i<num_bits; i++) {	// Clock num_bits out of the USI shift-register
		USICR |= _BV(USITC);				// Toggle the clock pin, falling edge. LCD samples the DO signal.
		asm volatile("nop\n\t"				// Delay so that the clock max frequency spec is not violated
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		::);
		USICR |= _BV(USITC);				// Toggle the clock pin, rising edge
		USICR |= _BV(USICLK);				// Strobe the USI shift-register and counter; this sets up the next data bit.
	}
}

/*
* Generate the Load pulse that causes the display driver chip to update the LCD segments with the
* current value of the driver chip's shift register.
*/
void lcd_update_display()
{
	PORTB |= _BV(PORTB1);		// Port B pin 1 high; Assert LCD Load
	asm volatile("nop\n\t"		// Delay so that the minimum load time spec is not violated
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	::);
	PORTB &= ~_BV(PORTB1);		// Port B pin 1 low; Deassert LCD Load
}

//////////////////////////////////////////////////////////////////////////
// Public variables and functions
//////////////////////////////////////////////////////////////////////////

/*
* Initialize the LCD. This assumes that the USI has already been initialized.
*/
void lcd_initialize()
{
	DDRB |= _BV(DDB1);									// Port B pin 1 is an output for LCD Load
	PORTB &= ~_BV(PORTB1);								// Port B pin 1 low; LCD Load low
}

// Array of LCD segment codes for the 10 numeric digits
const uint8_t lcd_segments_numeric[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

// Array of LCD segment codes for the 26 alphabetic characters
const uint8_t lcd_segments_alphabetic[26] = {
	0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x3d, 0x76,
	0x30, 0x1E, 0x76, 0x38, 0x15, 0x37, 0x3f, 0x73,
	0x67, 0x50, 0x6d, 0x78, 0x3e, 0x1C, 0x2A, 0x76,
	0x6e, 0x5b};

uint8_t lcd_segment_code(uint8_t ascii_code) {
	uint8_t segment_code = 0x00;									// Default is all segments off
	if (ascii_code >= 0x30 && ascii_code <= 0x39) {					// Numeric ascii codes: 0-9
		segment_code = lcd_segments_numeric[ascii_code - 0x30];
	}
	else if (ascii_code >= 0x41 && ascii_code <= 0x5A) {			// Uppercase alphabet: A-Z
		segment_code = lcd_segments_alphabetic[ascii_code - 0x41];
	}
	return segment_code;
}

void lcd_show_ascii_with_symbols(char* eight_char_buf, uint8_t symbols) {
	lcd_send_variable_bits(symbols, 4);								// Send symbols for right half LCD display
	
	uint8_t segment_code = 0x00;
	uint8_t char_idx=7;												// Start with rightmost char
	for (uint8_t num_chars=0; num_chars<4; num_chars++) {			// Send 4 chars to right half LCD display
		segment_code = lcd_segment_code(eight_char_buf[char_idx]);	// Get LCD segment code for the char
		lcd_send_variable_bits(segment_code, 7);					// Send 7-segment code to the LCD
		char_idx--;
	}
	
	lcd_send_variable_bits(symbols >> 4, 4);						// Send symbols for left half LCD display
	
	char_idx = 3;
	for (uint8_t num_chars=0; num_chars<4; num_chars++) {			// Send 4 chars to left half LCD display
		segment_code = lcd_segment_code(eight_char_buf[char_idx]);	// Get LCD segment code for the char
		lcd_send_variable_bits(segment_code, 7);					// Send 7-segment code to the LCD
		char_idx--;
	}
	
	lcd_update_display();											// Tell the LCD controller to update the visible segments
}

void lcd_show_ascii(char* eight_char_buf) {
	lcd_show_ascii_with_symbols(eight_char_buf, LCD_SYM_NONE);
}

void lcd_show_integer_with_symbols(uint32_t value, uint8_t symbols) {
	uint8_t bcd_buf[10];											// Buffer to hold 10 decimal digit values
	char ascii_buf[8];												// Buffer to hold 8 ascii-encoded digits
	uint8_t ascii_idx;
	bin_to_ten_dec_digits(value, bcd_buf);							// Convert binary value to 10 decimal digits
	for (ascii_idx=0; ascii_idx<8; ascii_idx++) {					// Copy decimal digits to ascii array
		ascii_buf[ascii_idx] = bcd_buf[ascii_idx+2] + 0x30;
	}
	lcd_show_ascii_with_symbols(ascii_buf, symbols);				// Show the ascii on the LCD
}

void lcd_show_integer(uint32_t value) {
	lcd_show_integer_with_symbols(value, LCD_SYM_NONE);
}

void lcd_clear() {
	lcd_show_ascii("        ");
}

void lcd_segment_test() {
	lcd_show_ascii_with_symbols("88888888", LCD_SYM_ALL);
}

//////////////////////////////////////////////////////////////////////////
// Test functions
//////////////////////////////////////////////////////////////////////////

void lcd_test_ordered()
{
		lcd_send_variable_bits(0x00, 4);	// sym: none
		lcd_send_variable_bits(0x7F, 7);	// 8
		lcd_send_variable_bits(0x07, 7);	// 7
		lcd_send_variable_bits(0x7D, 7);	// 6
		lcd_send_variable_bits(0x6D, 7);	// 5
		lcd_send_variable_bits(0x02, 4);	// sym: dp
		lcd_send_variable_bits(0x66, 7);	// 4
		lcd_send_variable_bits(0x4F, 7);	// 3
		lcd_send_variable_bits(0x5B, 7);	// 2
		lcd_send_variable_bits(0x06, 7);	// 1
		lcd_update_display();
}

void lcd_test_off()
{
	lcd_send_variable_bits(0x00, 4);	// sym: none
	lcd_send_variable_bits(0x00, 7);	// none
	lcd_send_variable_bits(0x00, 7);	// none
	lcd_send_variable_bits(0x00, 7);	// none
	lcd_send_variable_bits(0x00, 7);	// none
	lcd_send_variable_bits(0x00, 4);	// sym: none
	lcd_send_variable_bits(0x00, 7);	// none
	lcd_send_variable_bits(0x00, 7);	// none
	lcd_send_variable_bits(0x00, 7);	// none
	lcd_send_variable_bits(0x00, 7);	// none
	lcd_update_display();
}

void lcd_test_on()
{
	lcd_send_variable_bits(0x0F, 4);	// sym: all
	lcd_send_variable_bits(0x7F, 7);	// none
	lcd_send_variable_bits(0x7F, 7);	// none
	lcd_send_variable_bits(0x7F, 7);	// none
	lcd_send_variable_bits(0x7F, 7);	// none
	lcd_send_variable_bits(0x0F, 4);	// sym: all
	lcd_send_variable_bits(0x7F, 7);	// none
	lcd_send_variable_bits(0x7F, 7);	// none
	lcd_send_variable_bits(0x7F, 7);	// none
	lcd_send_variable_bits(0x7F, 7);	// none
	lcd_update_display();
}
