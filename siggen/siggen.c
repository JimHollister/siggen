#include <avr/io.h>
#include "common.h"
#include <util/delay.h>
#include <stdlib.h>
#include "pin.h"
#include "lcd.h"
#include "dds.h"
#include "bcd.h"

/*
* Initialization of the USI peripheral. USI is used to communicate with the DDS chip and the LCD.
*/
void usi_initialize()
{
	DDRB |= _BV(DDB6) | _BV(DDB7);							// DO and USCK as outputs
	USICR = _BV(USIWM0);									// USI in 3-wire mode, software clock strobe
}

int main(void)
{
	//_delay_ms(1000);
	
	pin_initialize();
	usi_initialize();
	lcd_initialize();
	dds_initialize();
	
	// Input tests
//	pin_test1();
//	pin_test2();
//	pin_test3();
//	pin_test4();
//	pin_test5();

	// DDS tests
//	dds_test3();
//	dds_test4();
//	dds_test5();

	//uint64_t bcd = 0;
	//bcd = uint32_to_bcd(0);
	//bcd = uint32_to_bcd(5);
	//bcd = uint32_to_bcd(123);
	//bcd = uint32_to_bcd(78681);
	//bcd = uint32_to_bcd(1000000);
	//bcd = uint32_to_bcd(87654321);
	//bcd = uint32_to_bcd(2147483647);
	//bcd = uint32_to_bcd(4294967295);
	
	uint8_t bcd_buf[8];
	//
	//bin_to_eight_dec_digits(0, bcd_buf);
	//bin_to_eight_dec_digits(1, bcd_buf);
	//bin_to_eight_dec_digits(20, bcd_buf);
	//bin_to_eight_dec_digits(100, bcd_buf);
	//bin_to_eight_dec_digits(101, bcd_buf);
	//bin_to_eight_dec_digits(123, bcd_buf);
	//bin_to_eight_dec_digits(1234, bcd_buf);
	//bin_to_eight_dec_digits(12345, bcd_buf);
	//bin_to_eight_dec_digits(99999999, bcd_buf);
	//
	uint32_t i=0;
	uint32_t j=0;
	for (; i<99999999; i++)
	{
		bin_to_eight_dec_digits(i, bcd_buf);
		j = eight_dec_digits_to_bin(bcd_buf);
		if (i != j) {
			abort();
		}
	}

	while(1)
	{
		unsigned long freq = 100000;
		
		// DDS and LCD communication tests
		PORTB &= ~_BV(PORTB7);		// Set USCK initially low to verify that it goes high before toggling
		while (1) {
// 			dds_test1();
// 			_delay_us(50);
//			lcd_test_off();
//			_delay_ms(1000);
//			lcd_test_ordered();
//			_delay_ms(1000);
//			lcd_test_on();
//			_delay_ms(1000);

			dds_set_frequency_integral(freq);
			//_delay_ms(1);
			_delay_us(100);
			freq = freq >= 15000000 ? 100000 : freq + 100;
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