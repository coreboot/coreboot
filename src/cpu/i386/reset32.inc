	.section ".reset"
	.code16
.globl	EXT(reset_vector)
EXT(reset_vector):

	. = 0x8;
	.code32
	jmp	EXT(protected_start)

	.previous
