/* boot.S 
 * assembler bootstrap
 *
 * Copyright (C) 2003 Stefan Reinauer
 *
 * See the file "COPYING" for further information about
 * the copyright and warranty status of this work.
 */

#define ASM 1
#include <multiboot.h>

	.text

	.code32
	.globl	start, _start

	/* unused */
start:
_start:
	/* Initialize stack pointer.  */
	movl	$(stack + STACK_SIZE), %esp

	/* Reset EFLAGS.  */
	pushl	$0
	popf

	/* parameter 2  */
	pushl	%ebx
	/* parameter 1  */
	pushl	%eax

	/* jump to C main function...  */
	call	EXT_C(cmain)

loop:	hlt
	jmp	loop

	.comm	stack, STACK_SIZE /* stack area.  */
