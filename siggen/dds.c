#include <avr/io.h>

// Initialization for the DDS chip
void dds_initialize() {
	DDRB |= (1 << DDB0);									// Port B pin 0 is an output for DDS Slave Select
	PORTB |= (1 << PORTB0);									// Port B pin 0 high; DDS chip SPI disabled
}

// Send 16 bits to the DDS using the SPI protocol
void dds_send_16_bits(uint16_t value) {
	uint8_t ms_byte = value >> 8;
	uint8_t ls_byte = value;
	
	PORTB |= (1 << PORTB7);					// Set USCK initially high. DDS expects this before chip select goes low.
	PORTB &= ~(1 << PORTB0);				// Port B pin 0 low, DDS chip selected.
	
	// Send the most-significant byte. Top bit goes first.
	USIDR = ms_byte;						// Load byte to be sent. This sets the DO line to the value of the top bit.
	for (uint8_t i=0; i<8; i++) {			// Clock 8 bits out of the USI shift-register
		USICR |= (1 << USITC);				// Toggle the clock pin, falling edge. DDS samples DO line.
		USICR |= (1 << USITC);				// Toggle the clock pin, rising edge
		USICR |= (1 << USICLK);				// Strobe the USI shift-register and counter; this sets up the next data bit.
	}
	// Send the least-significant byte. Top bit goes first.
	USIDR = ls_byte;						// Load byte to be sent. This sets the DO line to the value of the top bit.
	for (uint8_t i=0; i<8; i++) {			// Clock 8 bits out of of the USI shift-register
		USICR |= (1 << USITC);				// Toggle the clock pin, falling edge. DDS samples DO line.
		USICR |= (1 << USITC);				// Toggle the clock pin, rising edge
		USICR |= (1 << USICLK);				// Strobe the USI shift-register and counter; this sets up the next data bit.
	}
	
	PORTB |= (1 << PORTB0);					// Port B pin 0 high; SPI chip deselected
}