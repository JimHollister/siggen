#include <avr/io.h>
#include <avr/pgmspace.h>
#include "common.h"

const uint32_t divisors[8] PROGMEM = {10000000, 1000000, 100000, 10000, 1000, 100, 10};

void bin_to_eight_dec_digits(uint32_t binval, uint8_t* eight_byte_array) {
	uint32_t remainder = binval <= 99999999 ? binval : 99999999;	// Clip the input value if it's too large
	
	// For each of the top 7 digits, from most-significant to least-significant, determine the BCD digit value
	// by division of the input value by the decimal value associated with that digit. Perform the division by
	// successive subtraction to reduce code size.
	for (int arr_idx=0; arr_idx<7; arr_idx++) {				
		uint32_t divisor = pgm_read_dword(&divisors[arr_idx]);		// Retrieve the decimal value associated with digit
		uint8_t digit_value = 0;							// Start with a digit value of 0
		if (remainder >= divisor) {							// If the remaining value is < divisor, the digit value is 0.
			// Successively subtract the divisor value until the remainder is less then the divisor. Each subtraction
			// increments the digit value.
			do
			{
				digit_value++;
				remainder -= divisor;
			} while (remainder >= divisor);
		}
		// Done with this digit. Save it in the byte array.
		eight_byte_array[arr_idx] = digit_value;
	}
	
	// Done with the top 7 digits. The remainder is the digit value of the units digit.
	eight_byte_array[7] = remainder;
}


uint32_t eight_dec_digits_to_bin(uint8_t* eight_byte_array) {
	uint32_t binval = 0;
	
	for (int arr_idx=0; arr_idx<7; arr_idx++) {
		uint8_t digit_value = eight_byte_array[arr_idx];
		if (digit_value < 1) {
			continue;
		}
		uint32_t divisor = pgm_read_dword(&divisors[arr_idx]);
		while (digit_value > 0) {
			binval += divisor;
			digit_value--;
		}
	}
	
	binval += eight_byte_array[7];
	return binval;
}