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