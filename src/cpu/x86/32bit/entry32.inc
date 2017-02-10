/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* For starting coreboot in protected mode */

#include <arch/rom_segs.h>
#include <cpu/x86/post_code.h>
#include <rules.h>

	.code32


	/* This is the GDT for the ROM stage part of coreboot. It
	 * is different from the RAM stage GDT which is defined in
	 * c_start.S
	 */

	.align	4
.globl gdtptr
gdt:
gdtptr:
	.word	gdt_end - gdt -1 /* compute the table limit */
	.long	gdt		 /* we know the offset */
	.word	0

	/* selgdt 0x08, flat code segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xcf, 0x00 /* G=1 and 0x0f, So we get 4Gbytes for limit */

	/* selgdt 0x10,flat data segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00

	/* selgdt 0x18, flat code segment (64-bit) */
	.word   0xffff, 0x0000
	.byte   0x00, 0x9b, 0xaf, 0x00

gdt_end:


/*
 *	When we come here we are in protected mode. We expand
 *	the stack and copies the data segment from ROM to the
 *	memory.
 *
 *	After that, we call the chipset bootstrap routine that
 *	does what is left of the chipset initialization.
 *
 *	NOTE aligned to 4 so that we are sure that the prefetch
 *	cache will be reloaded.
 *
 *	In the bootblock there is already a ljmp to __protected_start and
 *	the reset vector jumps to symbol _start16bit in entry16.inc from
 *	the reset vectors's symbol which is _start. Therefore, don't
 *	expose the _start symbol for bootblock.
 */
	.align	4
#if !ENV_BOOTBLOCK
.globl _start
_start:
#endif

	lgdt	%cs:gdtptr
	ljmp	$ROM_CODE_SEG, $__protected_start

__protected_start:
	/* Save the BIST value */
	movl	%eax, %ebp

#if !IS_ENABLED(CONFIG_NO_EARLY_BOOTBLOCK_POSTCODES)
	post_code(POST_ENTER_PROTECTED_MODE)
#endif

	movw	$ROM_DATA_SEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* Restore the BIST value to %eax */
	movl	%ebp, %eax
