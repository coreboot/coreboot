/* mboot.S 
 * legacybios assembler bootstrap for multiboot
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
	jmp	multiboot_entry

	/* Align 32 bits boundary.  */
	.align	4
	
	/* Multiboot header.  */
multiboot_header:
	.long	MULTIBOOT_HEADER_MAGIC /* magic */
	.long	MULTIBOOT_HEADER_FLAGS /* flags */
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS) 
	        /* checksum */
	.long	multiboot_header /* header_addr */
	.long	_start /* load_addr */
	.long	_edata /* load_end_addr */
	.long	_end /* bss_end_addr */
	.long	multiboot_entry /* entry_addr */

multiboot_entry:
	/* Initialize stack pointer.  */
	movl	$(stack + STACK_SIZE), %esp

	/* Reset EFLAGS.  */
	pushl	$0
	popf

	/* pointer to multiboot information structure.  */
	pushl	%ebx
	/* magic value.  */
	pushl	%eax

	/* jump to C main function...  */
	call	EXT_C(cmain)

loop:	hlt
	jmp	loop

	.comm	stack, STACK_SIZE /* stack area.  */
