#include <avr/io.h>

// Initialization for the LCD
void lcd_initialize() {
	DDRB |= (1 << DDB1);									// Port B pin 1 is an output for LCD Load
	PORTB &= ~(1 << PORTB1);								// Port B pin 1 low; LCD Load low
}

// Send the specified number of bits to the LCD shift register. 1 <= num_bits <= 8.
// It's assumed that the bits are right-aligned in the value byte. For example, to send the 4-bit
// value 0b1010 to the LCD: value = 0x0A and num_bits = 4.
// The timing of operations in this function must meet the minimum timing constraints of the
// Microchip 80438 display driver. This functions assumes a system clock of 16.384 MHz and that this
// function has been compiled with optimization (-O1, -O2, -O3, or -Os). If the function is not compiled
// with optimization (-O0) the function will still work, but will send bits to the LCD more slowly.
void lcd_send_variable_bits(uint8_t value, uint8_t num_bits) {
	// Left align the value bits because the USI shift register sends the high bits first.
	for (uint8_t i=num_bits; i<8; i++) {
		value <<= 1;
	}

	PORTB |= _BV(PORTB7);					// Set USCK initially high

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

// Generates Load pulse that causes the display driver chip to update the LCD segments with the
// current value of the driver chip's shift register.
void lcd_update_display() {
	PORTB |= (1 << PORTB1);					// Port B pin 1 high; Assert LCD Load
	asm volatile("nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	"nop\n\t"
	::);
	PORTB &= ~(1 << PORTB1);				// Port B pin 1 low; Deassert LCD Load
}
