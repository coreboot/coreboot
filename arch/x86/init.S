# init code - switch cpu to pmode and enable cache as ram.
#
# Copyright (C) 2000 Ron Minnich, Advanced Computing Lab, LANL
# Copyright (C) 2007 Stefan Reinauer, coresystems GmbH
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
#


#include "macros.h"

#define ROM_CODE_SEG 0x08
#define ROM_DATA_SEG 0x10

#define CACHE_RAM_CODE_SEG 0x18
#define CACHE_RAM_DATA_SEG 0x20

	.code16
	.globl _start
_start:
	cli

	/* save the BIST result */
	movl	%eax, %ebp;

	/* thanks to kmliu@sis.tw.com for this TBL fix */
	/* IMMEDIATELY invalidate the translation lookaside buffer before
	 * executing any further code.  Even though paging is disabled we 
	 * could still get false address translations due to the TLB if we 
	 * didn't invalidate it.
	 */

	xorl	%eax, %eax
	movl	%eax, %cr3	/* Invalidate TLB */

	/* switch to protected mode */

	movw	%cs, %ax
	shlw	$4, %ax
	movw	$gdt16 + 0xe000, %bx
	subw	%ax, %bx
	data32	lgdt %cs:(%bx)

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	/* Restore BIST result */
	movl	%ebp, %eax


	// port80_post (0x23)     /* post 0x01 */
	/* Now we are in protected mode. Jump to a 32 bit code segment. */
	//data32 ljmp    $ROM_CODE_SEG, $__protected_start
	data32 ljmp    $ROM_CODE_SEG, $0xe058

	.align  4
	.globl gdt16
gdt16 = . - _start 
	.word   gdt_end - gdt -1 /* compute the table limit */
	.long   gdt              /* we know the offset */


	/* From now on we are 32bit */

	.code32


/* This is the gdt for ROMCC/ASM part of LinuxBIOS. It 
 * is different from the gdt in GCC part of LinuxBIOS.
 *
 * That on was defined in c_start.S in LinuxBIOS v2. TODO
 */

	.align  4
	.globl gdtptr
gdt:
gdtptr:
	.word   gdt_end - gdt -1 /* compute the table limit */
	.long   gdt              /* we know the offset */
	.word   0

	/* selgdt 0x08, flat code segment */
	.word   0xffff, 0x0000
	.byte   0x00, 0x9b, 0xcf, 0x00

	/* selgdt 0x10,flat data segment */
	.word   0xffff, 0x0000
	.byte   0x00, 0x93, 0xcf, 0x00
gdt_end:

/*
 *      When we come here we are in protected mode. We expand
 *      the stack and copies the data segment from ROM to the
 *      memory.
 *
 *      After that, we call the chipset bootstrap routine that
 *      does what is left of the chipset initialization.
 *
 *      NOTE aligned to 4 so that we are sure that the prefetch
 *      cache will be reloaded.
 */

	.align  4
#if 0
	//This code was used by v2. TODO
	.globl protected_start
protected_start:

	lgdt    %cs:gdtptr
	ljmp    $ROM_CODE_SEG, $__protected_start
#endif

.globl __protected_start
__protected_start:
	/* Save the BIST value */
	movl    %eax, %ebp

	port80_post (0x01)     /* post 0x01 */

	movw    $ROM_DATA_SEG, %ax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %ss
	movw    %ax, %fs
	movw    %ax, %gs

	/* Restore the BIST value to %eax */
	movl    %ebp, %eax

#define CONFIG_CPUTYPE_INTEL 1
#ifdef  CONFIG_CPUTYPE_INTEL
#include "cache_as_ram.S"
#endif

.align 4
