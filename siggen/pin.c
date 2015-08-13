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

void pin_test1()
{
	uint8_t previous_input = 0;
	uint8_t input = 0;
	uint8_t num_consecutive = 0;
	
	DDRD |= _BV(DDD5);					// Port D pin 5 is an output for testing purpose
	PORTD &= ~_BV(PORTD5);				// Port D pin 5 low
	
	while (1) {
		input = PIND & _BV(PIND6);
		if (input ^ previous_input) {
			num_consecutive = 0;
		} else {
			num_consecutive++;
			if (num_consecutive > 30) {
				// Enough consecutive values without a change has occurred. It's real.
				if (input) {					// Is Port D pin 6 high? Is switch open?
					PORTD &= ~_BV(PORTD5);		// Yes, set port D pin 5 low
					} else {
					PORTD |= _BV(PORTD5);		// No, set port D pin 5 high
				}
				num_consecutive = 0;
			}
		}
		previous_input = input;
		_delay_ms(1);
	}	
}