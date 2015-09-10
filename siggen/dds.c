/*
* Functions for handling the DDS chip.
*
* The timing of operations in this function must meet the minimum timing constraints of the
* Analog Devices AD9834 DDS chip. This functions assumes a system clock of 16.384 MHz and that this
* function has been compiled with optimization level -O1 or -Os). If the function is not compiled
* with optimization (-O0) the function will still work, but will send bits to the DDS more slowly.
*/

#include <avr/io.h>
#include "common.h"

extern unsigned long long mul_32x32(unsigned long multiplicand, unsigned long multiplier);

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

unsigned long tuning_freq_ratio = 0xE5109EC2;		// tuning/freq ratio of 3.57913941333333 in Q2.30 fixed point format

unsigned long calc_tuning_word_fractional(unsigned long output_freq) {
	// Desired output freq is assumed to be in 32-bit Q25.7 format
	// Tuning/freq ratio is in 32-bit Q2.30 fixed point format
	// Multiply output freq by tuning/ratio to get freq word in Q27.37 fixed point
	// Round this value to a 32-bit unsigned integer
	unsigned long long result = mul_32x32(output_freq, tuning_freq_ratio);	// Multiply and capture 64-bit result
	result = result >> 36;										// Right shift to get tuning word in Q63.1 fixed point
	bool roundup = false;
	if (result & 1) {roundup = 1;}								// Is the rightmost (1/2 place) bit set? If so, round up the final result
	unsigned long tuning_word = (unsigned long)(result >> 1);	// Right shift 1 time to get freq word in Q64.0 fixed point. Cast to Q32.0.
	if (roundup == true) {tuning_word++;}						// If called for, round the freq word up by one
	return tuning_word;
}

unsigned long calc_tuning_word_integral(unsigned long output_freq) {
	// Output freq value is in 32-bit unsigned long format, also known as Q32.0 fixed point format
	// Shift left to convert to 32-bit Q25.7 fixed point format and call the fractional function
	return calc_tuning_word_fractional(output_freq << 7);
}

void dds_test1()
{
	for (uint8_t i = 0; i<5; i++) {
		dds_send_16_bits(0x5aa5);
	}
}

void dds_test2() {
	volatile unsigned long freq_word = 0;
	
	freq_word = calc_tuning_word_integral(30000000);	// freq_word should be 107374182
	freq_word = calc_tuning_word_integral(1000000);	// freq_word should be 3579139
	freq_word = calc_tuning_word_integral(250000);		// freq_word should be 894785
	freq_word = calc_tuning_word_integral(1000);		// freq_word should be 3579
	freq_word = calc_tuning_word_integral(25);			// freq_word should be 89
	freq_word = calc_tuning_word_integral(4);			// freq_word should be 14
	freq_word = calc_tuning_word_integral(1);			// freq_word should be 4
	freq_word = calc_tuning_word_integral(0);			// freq_word should be 0
	
	freq_word = calc_tuning_word_fractional(0xAE9D85D9);	// desired freq = 22887179.6953125, freq_word should be 81916407
}

void dds_test3()
{
	// 800 kHz , measure at Rf out
	dds_send_16_bits(0x2100);
	dds_send_16_bits(0x70D0);
	dds_send_16_bits(0x40AE);
	dds_send_16_bits(0x2000);
}

void dds_test4()
{
	// 5 kHz, measure at audio out
	dds_send_16_bits(0x2100);
	dds_send_16_bits(0x45E8);
	dds_send_16_bits(0x4001);
	dds_send_16_bits(0x2000);
}

void dds_test5()
{
	// .279 Hz, measure at audio out
	dds_send_16_bits(0x2100);
	dds_send_16_bits(0x4001);
	dds_send_16_bits(0x4000);
	dds_send_16_bits(0x2000);
}

