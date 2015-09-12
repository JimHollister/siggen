/*
 * bcd.h
 *
 * Created: 9/11/2015 4:08:06 PM
 *  Author: Administrator
 */ 


#ifndef BCD_H_
#define BCD_H_

// Convert a 32-bit binary unsigned integer value to an eight-byte array where each array element is one BCD-encoded
// digit of the binary integer. For example, the binary integer with decimal value 12345 will result in the array:
// {0, 0, 0, 1, 2, 3, 4, 5}
// Maximum allowed value of the binary integer is 99999999. A larger value will be clipped to 99999999. 
void bin_to_eight_dec_digits(uint32_t binval, uint8_t* eight_byte_array);

// Reverse of bin_to_eight_dec_digits. Used to test that function.
uint32_t eight_dec_digits_to_bin(uint8_t* eight_byte_array);

#endif /* BCD_H_ */