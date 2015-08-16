/*
* Functions for handling the discrete pins connected to
* the pushbutton and the encoder inputs.
*/

#include <avr/io.h>
#include "common.h"
#include <util/delay.h>

/*
* Initialize for the inputs. 
*/

void pin_initialize()
{
	DDRD &= ~_BV(DDD6);					// Port D pin 6 is an input for the pushbutton
	PORTD |= _BV(PORTD6);				// Enable pullup resistor on port D pin 6 input
}

void pin_test_initialize()
{
	DDRD |= _BV(DDD5);					// Port D pin 5 is an output for testing purpose
	PORTD &= ~_BV(PORTD5);				// Port D pin 5 low
	DDRD |= _BV(DDD4);					// Port D pin 4 is an output for testing purpose
	PORTD &= ~_BV(PORTD4);				// Port D pin 5 low
}

void pin_test_update_indicator()
{
	if (PIND & _BV(PIND6)) {			// Is Port D pin 6 high? Is switch open?
			PORTD &= ~_BV(PORTD5);		// Yes, set port D pin 5 low
		} else {
			PORTD |= _BV(PORTD5);		// No, set port D pin 5 high
	}
}

void pin_test1()
{
	uint8_t previous_input = 0;
	uint8_t input = 0;
	uint8_t num_consecutive = 0;
	
	pin_test_initialize();
	
	while (1) {
		input = PIND & _BV(PIND6);
		if (input ^ previous_input) {
			num_consecutive = 0;
		} else {
			num_consecutive++;
			if (num_consecutive > 20) {
				// Enough consecutive values without a change has occurred. It's real.
				pin_test_update_indicator();
				num_consecutive = 0;
			}
		}
		previous_input = input;
		_delay_ms(1);
	}
}

void pin_test2()
{
	uint8_t previous_input = 0;
	uint8_t input = 0;
	uint8_t num_consecutive = 0;
	
	pin_test_initialize();
	
	while (1) {
		input = PIND & _BV(PIND6);
		if (input ^ previous_input) {
			if (num_consecutive >= 0) {
				num_consecutive--;
			}
		} else {
			num_consecutive++;
			if (num_consecutive > 20) {
				// Enough consecutive values without a change has occurred. It's real.
				pin_test_update_indicator();
				num_consecutive = 0;
			}
		}
		previous_input = input;
		_delay_ms(1);
	}	
}

void pin_test3()
{
	uint8_t state = 0;
	uint8_t y = 0;
	uint8_t x;
	
	pin_test_initialize();
	
	while (1) {
		PORTD |= _BV(PORTD4);				// Port D pin 4 high to indicate function has started
		
		// Read raw input and convert to low=0x00 and high=0xFF
		x = (PIND & _BV(PIND6)) > 0 ? 0x00 : 0xFF;
		
		// Filtered value is 7/8 of previous filtered value plus 1/8 of current raw value
		y = (y - (y >> 3)) + (x >> 3);
			
		// Compare filtered value to threshold values to see if a state change has occurred.
		if (state == 1 && y < 0x20) {
			state = 0;
			PORTD &= ~_BV(PORTD5);			// Port D pin 5 low to indicate state is now 0
		} else if (state == 0 && y > 0xE0) {
			state = 1;
			PORTD |= _BV(PORTD5);			// Port D pin 5 high to indicate state is now 1
		}
		
		PORTD &= ~_BV(PORTD4);				// Port D pin 4 low to indicate function has stopped
	
		_delay_us(500);
	}
}