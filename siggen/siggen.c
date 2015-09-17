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

//while (1) {
	//lcd_show_integer(12345678);
	//_delay_ms(1000);
	//lcd_show_integer_with_symbols(12345678, LCD_SYM_COLON_RIGHT);
	//_delay_ms(1000);
	//lcd_show_integer_with_symbols(12345678, LCD_SYM_DP2);
	//_delay_ms(1000);
	//lcd_show_integer_with_symbols(12345678, LCD_SYM_DP3);
	//_delay_ms(1000);
	//lcd_show_integer_with_symbols(12345678, LCD_SYM_DP4);
	//_delay_ms(1000);
	//lcd_show_integer_with_symbols(12345678, LCD_SYM_COLON_LEFT);
	//_delay_ms(1000);
	//lcd_show_integer_with_symbols(12345678, LCD_SYM_DP6);
	//_delay_ms(1000);
	//lcd_show_integer_with_symbols(12345678, LCD_SYM_DP7);
	//_delay_ms(1000);
	//lcd_show_integer_with_symbols(12345678, LCD_SYM_DP8);
	//_delay_ms(1000);
	//lcd_clear();
	//_delay_ms(5000);
	//lcd_show_integer(1234567);
	//_delay_ms(5000);
	//lcd_show_ascii("89ABCDEF");
	//_delay_ms(5000);
	//lcd_show_ascii("GHIJKLMN");
	//_delay_ms(5000);
	//lcd_show_ascii("OPQRSTUV");
	//_delay_ms(5000);
	//lcd_show_ascii("WXYZ    ");
	//_delay_ms(5000);
	//lcd_segment_test();
	//_delay_ms(5000);
//}


	//char ascii_buf[8];
	//for (uint8_t row=0; row<12; row++) {
		//for (uint8_t column=0; column<8; column++) {
			//uint8_t ascii_code = 0x20 + ((row << 3) + column);
			//ascii_buf[column] = ascii_code;
		//}
		//lcd_show_ascii(ascii_buf);
		//_delay_ms(5000);
	//}

	uint32_t i = 100000;
	while (1) {
//		_delay_ms(1);
		lcd_show_integer(i);
		dds_set_frequency_integral(i);
		i += 100;
		if (i > 2000000) {i = 100000;}
	}

	while(1)
	{
//		unsigned long freq = 100000;
		
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

			//dds_set_frequency_integral(freq);
			////_delay_ms(1);
			//_delay_us(100);
			//freq = freq >= 15000000 ? 100000 : freq + 100;
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