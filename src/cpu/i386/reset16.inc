	.section ".reset"
	.code16
.globl	EXT(reset_vector)
EXT(reset_vector):
#if _ROMBASE >= 0xffff0000
	/* Hmm.
	 * _start_offset is the low 16 bits of _start.
	 * Theoretically we should have problems but it compiles
	 * and links properly with binutils 2.9.5 & 2.10.90
	 * This is probably a case that needs fixing in binutils.
	 * And then we can just use _start.
	 * We also need something like the assume directive in
	 * other assemblers to tell it where the segment registers
	 * are pointing in memory right now.
	 */
	jmp	EXT(_start_offset)
#elif (_ROMBASE < 0x100000)
	ljmp	$((_ROMBASE & 0xf0000)>>4),$EXT(_start_offset);
#else
#error	_ROMBASE is an unsupported value
#endif

	. = 0x8;
	.code32
	jmp	EXT(protected_start)

	.previous
