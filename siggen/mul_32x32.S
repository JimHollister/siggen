
#define _SFR_ASM_COMPAT 1
#define __SFR_OFFSET 0      // Use 0 for the I/O register offset
#include <avr/io.h>         // Define I/O port aliases
  
.section .text              // Define a code section

.global mul_32x32			// Make mul_32x32 visible to other source files

// Perform shift-and-add multiplication of two unsigned 32-bit values to create
// an unsigned 64-bit result.
mul_32x32:

#define	ANS1 R18	// 64-bit answer
#define ANS2 R19
#define ANS3 R20
#define ANS4 R21
#define ANS5 R22
#define ANS6 R23
#define ANS7 R24
#define ANS8 R25

#define MPCND1 R14		// multiplicand
#define MPCND2 R15
#define MPCND3 R16
#define MPCND4 R17

#define MLTPR1 R18		// multiplier, stored in lower 4 bytes of answer
#define MLTPR2 R19
#define MLTPR3 R20
#define MLTPR4 R21

#define C R30		// loop counter

initialize:
	// When called from C:
	// arg1, multiplicand: R25(msb),R24,R23,R22(lsb)
	// arg2, multiplier: R21(msb),R20,R19,R18(lsb)
	// return, answer: R25(msb),R24,R23,R22,R21,R20,R19,R18(lsb)
	//
	// register plan:
	// copy multiplicand to R17(msb) through R14(lsb)
	// calculate answer in R25(msb) through R18(lsb)
	// bottom 4 bytes of answer already initialized as multiplier (by C call)

	// Save previous R14-R17 and copy multiplicand to those registers
	push	R14
	push	R15
	push	R16
	push	R17
	mov		R14,R22
	mov		R15,R23
	mov		R16,R24
	mov		R17,R25

	// Initialize answer registers
	// Bottom 4 bytes, multiplier, are already initialized by C call. Clear top 4 bytes and carry flag.
	clr		ANS5
	clr		ANS6
	clr		ANS7
	sub		ANS8,ANS8	// Clears ANS8 and carry flag

	// Initialize the loop counter to 33
	ldi		C,33

loop:
	// Shift multiplier (low bytes of answer) right one bit
	ror		MLTPR4
	ror		MLTPR3
	ror		MLTPR2
	ror		MLTPR1		// previous bit0 -> carry bit

	dec		C			// Decrement loop counter, doesn't affect carry bit
	breq	done		// If all bits processed, we're done multiplying
	brcc	skip_add	// If carry bit is clear, skip addition

	// Add multiplicand to answer
add:
	add		ANS5,MPCND1
	adc		ANS6,MPCND2
	adc		ANS7,MPCND3
	adc		ANS8,MPCND4

skip_add:
	// Shift high bytes of answer right one bit
	ror		ANS8
	ror		ANS7
	ror		ANS6
	ror		ANS5

	rjmp	loop			// Loop back and process another bit

done:
restore_regs:
	// Restore previous R14-R17
	pop		R17
	pop		R16
	pop		R15
	pop		R15

return:
	ret
