	.section ".reset"
	.code16
.globl	reset_vector
reset_vector:
#if _ROMBASE >= 0xffff0000
	/* jmp _start */
	.byte  0xe9
	.int   _start - ( . + 2 )
	/* Note: The above jump is hand coded to work around bugs in binutils.
	 * 5 byte are used for a 3 byte instruction.  This works because x86
	 * is little endian and allows us to use supported 32bit relocations
	 * instead of the weird 16 bit relocations that binutils does not
	 * handle consistenly between versions because they are used so rarely.
	 */
#else
# error _ROMBASE is an unsupported value
#endif
	. = 0x8;
	.code32
	jmp	protected_start
	.previous
