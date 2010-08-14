/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 1999 Ronald G. Minnich
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#include <arch/rom_segs.h>
	.code16
	.globl _stage0
_stage0:
	cli

	/* Save the BIST result. */
	movl	%eax, %ebp;

	/* thanks to kmliu@sis.com.tw for this TLB fix */
	/* IMMEDIATELY invalidate the translation lookaside buffer (TLB) before
	 * executing any further code. Even though paging is disabled we
	 * could still get false address translations due to the TLB if we
	 * didn't invalidate it.
	 */
	xorl	%eax, %eax
	movl	%eax, %cr3	/* Invalidate TLB. */

	/* Switch to protected mode. */

	/* NOTE: With GNU assembler version 2.15.94.0.2.2 (i386-redhat-linux)
	 * using BFD version 2.15.94.0.2.2 20041220 this works fine without all
	 * the ld hackery and other things. So leave it as is with this comment.
	 */

	data32	lgdt %cs:gdtptr

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG, AM, WP, NE, TS, EM, MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	/* Restore BIST result. */
	movl	%ebp, %eax

	// port80_post(0x23)
	/* Now we are in protected mode. Jump to a 32 bit code segment. */
	data32	ljmp $ROM_CODE_SEG, $protected_stage0

	/* I am leaving this weird jump in here in the event that future gas
	 * bugs force it to be used.
	 */
	/* .byte 0x66 */
	.code32
	/* ljmp	$ROM_CODE_SEG, $protected_stage0 */

	/* .code16 */
	.align 4
	.globl gdt16
gdt16 = . - _stage0
gdt16x:
	.word	gdt16xend - gdt16x -1	/* Compute the table limit. */
	.long	gdt16x
	.word	0

	/* selgdt 0x08, flat code segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xcf, 0x00

	/* selgdt 0x10, flat data segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00
gdt16xend:

	/* From now on we are 32 bit. */
	.code32

	/* We have two gdts where we could have one. That is ok.
	 *
	 * Let's not worry about this -- optimizing gdt is pointless since
	 * we're only in it for a little bit.
	 *
	 * Btw. note the trick below: The GDT points to ITSELF, and the first
	 * good descriptor is at offset 8. So you word-align the table, and
	 * then because you chose 8, you get a nice 64-bit aligned GDT entry,
	 * which is good as this is the size of the entry.
	 * Just in case you ever wonder why people do this.
	 */
	.align 4
	.globl gdtptr
	.globl gdt_limit
gdt_limit = gdt_end - gdt - 1		/* Compute the table limit. */

gdt:
gdtptr:
	.word	gdt_end - gdt -1	/* Compute the table limit. */
	.long	gdt			/* We know the offset. */
	.word	0

	/* selgdt 0x08, flat code segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xcf, 0x00

	/* selgdt 0x10, flat data segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00

gdt_end:

/* Reset vector. */

/*
 * RVECTOR: Size of reset vector, default is 0x10.
 * RESRVED: Size of vpd code, default is 0xf0.
 * BOOTBLK: Size of bootblock code, default is 0x1f00 (8k-256b).
 */

SEGMENT_SIZE = 0x10000
RVECTOR      = 0x00010

/* Due to YET ANOTHER BUG in GNU bintools, you can NOT have a code16 here.
 * I think we should leave it this way forever, as the bugs come and
 * go -- and come again.
 *
 *	.code16
 *	.section ".rom.text"
 */
.section ".reset", "ax"
	.globl _resetjump
_resetjump:
	/* GNU bintools bugs again. This jumps to stage0 - 2. Sigh. */
	/* jmp _stage0 */
	.byte	0xe9
	.int	_stage0 - ( . + 2 )

	/* Note: The above jump is hand coded to work around bugs in binutils.
	 * 5 bytes are used for a 3 byte instruction. This works because x86
	 * is little endian and allows us to use supported 32 bit relocations
	 * instead of the weird 16 bit relocations that binutils does not
	 * handle consistenly between versions because they are used so rarely.
	 */
