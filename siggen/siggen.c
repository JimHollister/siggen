#include <stdbool.h>
#include <avr/io.h>
#include "common.h"
#include <util/delay.h>
#include "pin.h"
#include "lcd.h"
#include "dds.h"
//#include <stdfix.h>

extern unsigned long long mul_32x32(uint32_t multiplicand, uint32_t multiplier);

/*
* Initialization of the USI peripheral. USI is used to communicate with the DDS chip and the LCD.
*/
void usi_initialize()
{
	DDRB |= _BV(DDB6) | _BV(DDB7);							// DO and USCK as outputs
	USICR = _BV(USIWM0);									// USI in 3-wire mode, software clock strobe
}

/*
* word = (freq * 2^28) / 75000000
* 2^28 = 268435456
* 2^28 / 75000000 = 3.57913941333333
* Word/Freq ratio in 32-bit Q2.30. Range is 0-3.999999999068677425384521484375 with resolution of 0.000000000931322574615478515625
* In Q2.30 = 11.100101000100001001111011000010 = 0xE5109EC2
* Converts back to  3.57913941331207. Difference is 0.00000000002126 or 0.0000000006%
* Desired freq in 32-bit Q25.7. Range is 0-33554431.9921875 Hz with resolution of 0.0078125 Hz
* Multiply Q25.7 by Q2.30 results in a 64-bit Q27.37. Convert back to a Q32.0 integer (with rounding)
*/

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

int main(void)
{
//	pin_initialize();
	usi_initialize();
	lcd_initialize();
//	dds_initialize();
	
	// Input tests
//	pin_test1();
//	pin_test2();
//	pin_test3();
//	pin_test4();
//	pin_test5();

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

	volatile unsigned long long result;

	// Answer should be 0
	result = mul_32x32(0x12345678, 0);

	// Answer should be 305419896
	result = mul_32x32(0x12345678, 1);

	// Answer should be 693779765864729976	
	result = mul_32x32(0x12345678, 0x87654321);

	// Answer should be 9223372036854775807
	result = mul_32x32(0xFFFFFFFF, 0xFFFFFFFF);

	while(1)
	{
		// DDS and LCD communication tests
		PORTB &= ~_BV(PORTB7);		// Set USCK initially low to verify that it goes high before toggling
		while (1) {
// 			dds_test1();
// 			_delay_us(50);
			lcd_test_off();
			_delay_ms(1000);
			lcd_test_ordered();
			_delay_ms(1000);
			lcd_test_on();
			_delay_ms(1000);
		}

		//for (uint16_t i = 0; i<1000; i++) {}
		//	    if (num_interrupts == 29) {
		//		    PORTB |= (1 << PORTB0);			// Port B pin 0 on
		//	    } else if (num_interrupts == 30) {
		//			PORTB &= ~(1 << PORTB0);		// Port B pin 0 off
		//			num_interrupts = 0;
		//			USIDR = 0x81;
		//		}
	}
}