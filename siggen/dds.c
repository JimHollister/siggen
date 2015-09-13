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

//////////////////////////////////////////////////////////////////////////
// Private variables and functions
//////////////////////////////////////////////////////////////////////////

// External assembly function for multiplying two 32-bit unsigned integers to get a 64-bit unsigned result
extern unsigned long long mul_32x32(unsigned long multiplicand, unsigned long multiplier);

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

// DDS control word bit usage:
//
// DB15,DB14 = 00 : Register address = Control
// DB13 = 1 : B28 = 1, consecutive writes will load freq lsb, msb
// DB12 = 0 : HLB = 0, ignored since B28=1
// DB11 = 0 : FSEL = 0 or 1 depending on which frequency register is being used
// DB10 = 0 : PSEL = 0 or 1 depending on which phase register is being used
// DB9 = 1 : PIN/SW = 0, reset, sleep, and freq/phase reg select will be controlled by software instead of pins
// DB8 = 0 : RESET = 0, high during initialization to reset DDS
// DB7 = 0 : SLEEP1 = 0, sleep mode not used
// DB6 = 0 : SLEEP12 = 0, sleep mode not used
// DB5 = 0 : OPBITEN = 0,, sign bit output not used
// DB4 = 0 : SIGN/PIB = 0, sign bit output not used
// DB3 = 0 : DIV2 = 0, sign bit output not used
// DB2 = 0 : Reserved, must be 0
// DB1 = 0 : MODE = 0, sine lookup table is used
// DB0 = 0 : Reserved, must be 0

// Alternate between using freq0/phase0 register set and freq1/phase1 register set so that the DDS chip continues producing output specified
// by one register set while the other register set is being loaded for the next frequency/phase output.
uint8_t dds_register_set = 0;								// Next freq/phase register set to use: 0 or 1, alternates with each freq change
const uint16_t dds_control_reset_bit = 0x0100;				// Control register bit to put DDS into reset state
const uint16_t dds_control_words[2] = {0x2000, 0x2C00};		// Control word for the two register sets
const uint16_t dds_freq_addr_bits[2] = {0x4000, 0x8000};	// Register addr bits for frequency registers
const uint16_t dds_phase_addr_bits[2] = {0xC000, 0xE000};	// Register addr bits for phase registers

// Change the DDS frequency by giving it a new tuning word to add to the phase accumulator
void dds_change_frequency(unsigned long tuning_word) {
	uint32_t tuning_bits = (uint32_t)(tuning_word & 0x0FFFFFFF);			// Mask tuning value to lower 28 bits only
	
//	dds_send_16_bits(dds_control_words[dds_register_set] | dds_control_reset_bit);	// Load control word that identifies register set to use
		
	uint16_t freq_word = (uint16_t)(tuning_bits & 0x00003FFF);				// Get least-significant 14 bits of tuning value
	dds_send_16_bits(dds_freq_addr_bits[dds_register_set] | freq_word);		// Set top two bits to register address and send freq LSBs to DDS
		
	freq_word = (uint16_t)(tuning_bits >> 14);								// Get most-significant 14 bits of tuning value
	dds_send_16_bits(dds_freq_addr_bits[dds_register_set] | freq_word);		// Set top two bits to register address and send freq MSBs to DDS
		
	dds_send_16_bits(dds_control_words[dds_register_set]);					// Load control word that identifies register set to use
	
	dds_register_set = dds_register_set == 0 ? 1 : 0;						// Select the register set to use next time
}

// Multiply the desired frequency by this ratio to get the tuning word. Equal to 2^28 / 75000000.
const unsigned long dds_tuning_freq_ratio = 0xE5109EC2;		// tuning/freq ratio of 3.57913941333333 in Q2.30 fixed point format

// Calculate a DDS tuning word given a desired output frequency in unsigned Q25.7 fixed point format
unsigned long dds_calc_tuning_word_fractional(unsigned long output_freq) {
	// Desired output freq is assumed to be in 32-bit Q25.7 format
	// Tuning/freq ratio is in 32-bit Q2.30 fixed point format
	// Multiply output freq by tuning/ratio to get freq word in Q27.37 fixed point
	// Round this value to a 32-bit unsigned integer
	unsigned long long result = mul_32x32(output_freq, dds_tuning_freq_ratio);	// Multiply and capture 64-bit result
	result = result >> 36;										// Right shift to get tuning word in Q63.1 fixed point
	bool roundup = false;
	if (result & 1) {roundup = 1;}								// Is the rightmost (1/2 place) bit set? If so, round up the final result
	unsigned long tuning_word = (unsigned long)(result >> 1);	// Right shift 1 time to get freq word in Q64.0 fixed point. Cast to Q32.0.
	if (roundup == true) {tuning_word++;}						// If called for, round the freq word up by one
	return tuning_word;
}

// Calculate a DDS tuning word given a desired output frequency in unsigned 32-bit integer format
unsigned long dds_calc_tuning_word_integral(unsigned long output_freq) {
	// Output freq value is in 32-bit unsigned long format, also known as Q32.0 fixed point format
	// Shift left to convert to 32-bit Q25.7 fixed point format and call the fractional function
	return dds_calc_tuning_word_fractional(output_freq << 7);
}

//////////////////////////////////////////////////////////////////////////
// Public variables and functions
//////////////////////////////////////////////////////////////////////////

// Initialize the MCU for communicating with the DDS and then initialize the DDS. Called once at startup.
void dds_initialize()
{
	DDRB |= _BV(DDB0);													// Port B pin 0 is an output for DDS Slave Select
	PORTB |= _BV(PORTB0);												// Port B pin 0 high; DDS chip SPI disabled
	
	dds_send_16_bits(dds_control_words[0] | dds_control_reset_bit);		// Load control word that puts DDS in reset state
	dds_send_16_bits(dds_freq_addr_bits[0] | 0x0000);					// Zero the freq0 LSB
	dds_send_16_bits(dds_freq_addr_bits[0] | 0x0000);					// Zero the freq0 MSB
	dds_send_16_bits(dds_phase_addr_bits[0] | 0x0000);					// Zero the phase0 register
	dds_send_16_bits(dds_freq_addr_bits[1] | 0x0000);					// Zero the freq1 LSB
	dds_send_16_bits(dds_freq_addr_bits[1] | 0x0000);					// Zero the freq1 MSB
	dds_send_16_bits(dds_phase_addr_bits[1] | 0x0000);					// Zero the phase1 register
	dds_register_set = 0;												// Set which frequency and phase registers to use next
}

// Set the DDS output to a frequency specified in unsigned Q25.7 fixed point format
void dds_set_frequency_fractional(unsigned long frequency) {
	unsigned long tuning_word = dds_calc_tuning_word_fractional(frequency);
	dds_change_frequency(tuning_word);
}

// Set the DDS output to a frequency specified in unsigned 32 bit integer format
void dds_set_frequency_integral(unsigned long frequency) {
	unsigned long tuning_word = dds_calc_tuning_word_integral(frequency);
	dds_change_frequency(tuning_word);
}

//////////////////////////////////////////////////////////////////////////
// Test functions
//////////////////////////////////////////////////////////////////////////

void dds_test1()
{
	for (uint8_t i = 0; i<5; i++) {
		dds_send_16_bits(0x5aa5);
	}
}

void dds_test2() {
	//volatile unsigned long tuning_word = 0;
	//
	//tuning_word = dds_calc_tuning_word_integral(30000000);	// freq_word should be 107374182
	//tuning_word = dds_calc_tuning_word_integral(1000000);		// freq_word should be 3579139
	//tuning_word = dds_calc_tuning_word_integral(250000);		// freq_word should be 894785
	//tuning_word = dds_calc_tuning_word_integral(1000);		// freq_word should be 3579
	//tuning_word = dds_calc_tuning_word_integral(25);			// freq_word should be 89
	//tuning_word = dds_calc_tuning_word_integral(4);			// freq_word should be 14
	//tuning_word = dds_calc_tuning_word_integral(1);			// freq_word should be 4
	//tuning_word = dds_calc_tuning_word_integral(0);			// freq_word should be 0
	//
	//tuning_word = dds_calc_tuning_word_fractional(0xAE9D85D9);	// desired freq = 22887179.6953125, freq_word should be 81916407
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
	dds_send_16_bits(0x2100);
	dds_send_16_bits(0x45E8);
	dds_send_16_bits(0x4002);
	dds_send_16_bits(0x2000);
	
	// .279 Hz, measure at audio out
	//dds_send_16_bits(0x2100);
	//dds_send_16_bits(0x4001);
	//dds_send_16_bits(0x4000);
	//dds_send_16_bits(0x2000);
}

