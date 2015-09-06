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

unsigned long word_freq_ratio = 0xE5109EC2;		// Word/freq ratio of 3.57913941333333 in Q2.30 fixed point

unsigned long calc_freq_word(unsigned long freq) {
	// Supplied freq value is in unsigned 32-bit format, also known as Q32.0 fixed point
	unsigned long long temp = mul_32x32(freq, word_freq_ratio);		// Multiply freq by ratio to get freq word value in Q34.30 fixed point
	temp = temp >> 29;												// Right shift 29 times to get freq word in Q63.1 fixed point
	bool roundup = false;											// Is the rightmost (1/2 place) bit set? If so, round up the final result
	if (temp & 0x000001) {roundup = 1;}
	unsigned long freq_word = (unsigned long)(temp >> 1);	// Right shift 1 time to get freq word in Q64.0 fixed point. Cast to Q32.0.
	if (roundup == true) {freq_word++;}						// If called for, round the freq word up by one
	return freq_word;
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
	freq_word = calc_freq_word(30000000);	// freq_word should be 107374182
	freq_word = calc_freq_word(1000000);	// freq_word should be 3579139
	freq_word = calc_freq_word(250000);		// freq_word should be 894785
	freq_word = calc_freq_word(1000);		// freq_word should be 3579
	freq_word = calc_freq_word(25);			// freq_word should be 89
	freq_word = calc_freq_word(4);			// freq_word should be 14
	freq_word = calc_freq_word(1);			// freq_word should be 4
	freq_word = calc_freq_word(0);			// freq_word should be 0

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