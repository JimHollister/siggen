#ifndef BCD_H_
#define BCD_H_

// Convert a 32-bit binary unsigned integer value to a ten-byte array where each array element is one BCD-encoded
// digit decimal equivalent. For example, the binary integer with decimal value 12345 will result in the array:
// {0, 0, 0, 0, 0, 1, 2, 3, 4, 5}
// Performance: 305 clock cycles (18.62us) to convert 0x00000000. 877 clock cycles (53.53us) to convert 0xFFFFFFFF.
// Code size: 150 bytes
void bin_to_ten_dec_digits(uint32_t binval, uint8_t* ten_byte_array);

// Reverse of bin_to_eight_dec_digits. Used to test that function.
uint32_t ten_dec_digits_to_bin(uint8_t* ten_byte_array);

#endif /* BCD_H_ */