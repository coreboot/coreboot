/*
 * $ $
 *
 */

#include <arch/asm.h>
#include <arch/intel.h>

.section ".rom.data", "a", @progbits
.section ".rom.text", "ax", @progbits

#include "crt0_includes.h"

	.globl _start
_start:	

	/* set new stack */
	movl	$_estack, %esp
	call	EXT(standalonemain)
	/*NOTREACHED*/
.Lhlt:	
	intel_chip_post_macro(0xee)	/* post fe */
	hlt
	jmp	.Lhlt

