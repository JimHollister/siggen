
#define _SFR_ASM_COMPAT 1
#define __SFR_OFFSET 0          // Use 0 for the I/O register offset
#include <avr/io.h>             // Defines I/O port aliases
  
.section .text              ; Defines a code section

.global mul_32x32			; Makes mul_32x32_r64 visible in other source files

#define	ANS1 R0		// 64-bit answer
#define ANS2 R1
#define ANS3 R2
#define ANS4 R3
#define ANS5 R4
#define ANS6 R5
#define ANS7 R6
#define ANS8 R7

#define A1 R16		// multiplicand
#define A2 R17
#define A3 R18
#define A4 R19

#define B1 R20		// multiplier
#define B2 R21
#define B3 R22
#define B4 R23

#define C R24		// loop counter

mul_32x32:
initialize:

// Temporarily load multiplicand and multiplier with constants
// Get these from C function call args in the future
	ldi		A1,0xff
	ldi		A2,0xff
	ldi		A3,0xff
	ldi		A4,0xff
	ldi		B1,0xff
	ldi		B2,0xff
	ldi		B3,0xff
	ldi		B4,0xff

	// Initialize answer to zero
	clr		ANS1
	clr		ANS2
	clr		ANS3
	clr		ANS4
	clr		ANS5
	clr		ANS6
	clr		ANS7
	sub		ANS8,ANS8	// Clears ANS8 and carry flag

	// Copy multiplier to answer low bytes
	mov		ANS1,B1
	mov		ANS2,B2
	mov		ANS3,B3
	mov		ANS4,B4

	ldi		C,33		// Initialize loop counter to 33

loop:
	// Shift multiplier right one bit
	ror		ANS4
	ror		ANS3
	ror		ANS2
	ror		ANS1

	dec		C			// Decrement loop counter
	breq	done		// If all bits processed, we're done multiplying
	brcc	skip		// If carry bit is clear, skip addition

	// Add multiplicand into answer
	add		ANS5,A1
	adc		ANS6,A2
	adc		ANS7,A3
	adc		ANS8,A4

skip:
	// Shift high bytes of answer
	ror		ANS8
	ror		ANS7
	ror		ANS6
	ror		ANS5

	rjmp	loop			// Loop back and process another bit

done:
	ret
