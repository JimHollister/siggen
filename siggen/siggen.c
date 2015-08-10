#include <avr/io.h>
//#include <avr/power.h>
#include <avr/interrupt.h>

#define F_CPU 16384000UL	// 16.384 MHz
#include <util/delay.h>

#include "lcd.h"
#include "dds.h"

volatile uint16_t num_interrupts = 0;

// Interrupt service routine for timer 0 output compare match interrupt
ISR(TIMER0_COMPA_vect) {
	num_interrupts++;
}

// Initialization of the USI peripheral. USI is used to communicate with the DDS chip and the LCD
void usi_initialize() {
	DDRB |= (1 << DDB6) | (1 << DDB7);						// DO and USCK as outputs
	USICR = (1 << USIWM0);									// USI in 3-wire mode, software clock strobe
}

int main(void)
{
	while(1)
	{
		//PORTB &= ~(1 << DDB7);					// USCK initially low
		//for (uint8_t i = 0; i<5; i++) {
		//dds_send_16_bits(0x5aa5);
		//}
		//for (uint16_t i = 0; i<1000; i++) {}
		
		PORTB &= ~(1 << PORTB7);					// USCK initially low for testing
		
		lcd_send_variable_bits(0x00, 4);
		lcd_send_variable_bits(0x7F, 7);
		lcd_send_variable_bits(0x07, 7);
		lcd_send_variable_bits(0x7D, 7);
		lcd_send_variable_bits(0x6D, 7);
		lcd_send_variable_bits(0x02, 4);
		lcd_send_variable_bits(0x66, 7);
		lcd_send_variable_bits(0x4F, 7);
		lcd_send_variable_bits(0x5B, 7);
		lcd_send_variable_bits(0x06, 7);
		lcd_update_display();
		
		_delay_us(10);
		
		
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