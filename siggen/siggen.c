#include <avr/io.h>
#include <avr/interrupt.h>
#include "common.h"
#include <util/delay.h>
#include "lcd.h"
#include "dds.h"

volatile uint16_t num_interrupts = 0;

// Interrupt service routine for timer 0 output compare match interrupt
ISR(TIMER0_COMPA_vect)
{
	num_interrupts++;
}

/*
* Initialization of the USI peripheral. USI is used to communicate with the DDS chip and the LCD.
*/
void usi_initialize()
{
	DDRB |= (1 << DDB6) | (1 << DDB7);						// DO and USCK as outputs
	USICR = (1 << USIWM0);									// USI in 3-wire mode, software clock strobe
}

int main(void)
{
	usi_initialize();
	lcd_initialize();
	dds_initialize();

	while(1)
	{
		lcd_test1();
//		dds_test1();

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