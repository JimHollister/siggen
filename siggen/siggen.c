#include <avr/io.h>
#include "common.h"
#include <util/delay.h>
#include "pin.h"
#include "lcd.h"
#include "dds.h"
//#include <stdfix.h>

extern void mul_32x32();

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

// Store ratio in a Q32.32 fixed point variable
//volatile unsigned long accum ratio = 3.57913941333333lk;
//volatile unsigned accum ratio = 3.57913941333333lk;
// Desired frequency is also in a Q32.32 fixed point variable
//volatile unsigned long accum freq = 12345678lk;
//volatile unsigned accum freq = 1234lk;
// Calculate frequency word value
//volatile unsigned long accum word = ratio * freq;

	mul_32x32();

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