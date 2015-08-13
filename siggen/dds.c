/*
* Functions for handling the DDS chip.
*
* The timing of operations in this function must meet the minimum timing constraints of the
* Analog Devices AD9834 DDS chip. This functions assumes a system clock of 16.384 MHz and that this
* function has been compiled with optimization level -O1 or -Os). If the function is not compiled
* with optimization (-O0) the function will still work, but will send bits to the DDS more slowly.
*/

#include <avr/io.h>

/*
* Initialize the DDS. This assumes that the USI has already been initialized.
*/
void dds_initialize()
{
	DDRB |= _BV(DDB0);									// Port B pin 0 is an output for DDS Slave Select
	PORTB |= _BV(PORTB0);								// Port B pin 0 high; DDS chip SPI disabled
}

// Send 16 bits to the DDS using the SPI protocol
void dds_send_16_bits(uint16_t value)
{
	uint8_t ms_byte = value >> 8;
	uint8_t ls_byte = value;
	
	PORTB |= _BV(PORTB7);					// Set USCK initially high. DDS expects this before chip select goes low.
	PORTB &= ~_BV(PORTB0);				// Port B pin 0 low, DDS chip selected.
	
	// Send the most-significant byte. Top bit goes first.
	USIDR = ms_byte;						// Load byte to be sent. This sets the DO line to the value of the top bit.
	for (uint8_t i=0; i<8; i++) {			// Clock 8 bits out of the USI shift-register
		USICR |= _BV(USITC);				// Toggle the clock pin, falling edge. DDS samples DO line.
		USICR |= _BV(USITC);				// Toggle the clock pin, rising edge
		USICR |= _BV(USICLK);				// Strobe the USI shift-register and counter; this sets up the next data bit.
	}
	// Send the least-significant byte. Top bit goes first.
	USIDR = ls_byte;						// Load byte to be sent. This sets the DO line to the value of the top bit.
	for (uint8_t i=0; i<8; i++) {			// Clock 8 bits out of of the USI shift-register
		USICR |= _BV(USITC);				// Toggle the clock pin, falling edge. DDS samples DO line.
		USICR |= _BV(USITC);				// Toggle the clock pin, rising edge
		USICR |= _BV(USICLK);				// Strobe the USI shift-register and counter; this sets up the next data bit.
	}
	
	PORTB |= _BV(PORTB0);					// Port B pin 0 high; SPI chip deselected
}

void dds_test1()
{
	for (uint8_t i = 0; i<5; i++) {
		dds_send_16_bits(0x5aa5);
	}
}