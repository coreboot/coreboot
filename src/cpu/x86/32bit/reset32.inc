	.section ".reset"
	.code16
.globl	reset_vector
reset_vector:

	. = 0x8;
	.code32
	jmp	protected_start

	.previous
