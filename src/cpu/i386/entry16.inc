/*
This software and ancillary information (herein called SOFTWARE )
called LinuxBIOS          is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
been authored by an employee or employees of the University of
California, operator of the Los Alamos National Laboratory under
Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
U.S. Government has rights to use, reproduce, and distribute this
SOFTWARE.  The public may copy, distribute, prepare derivative works
and publicly display this SOFTWARE without charge, provided that this
Notice and any statement of authorship are reproduced on all copies.
Neither the Government nor the University makes any warranty, express
or implied, or assumes any liability or responsibility for the use of
this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
such modified SOFTWARE should be clearly marked, so as not to confuse
it with the version available from LANL.
 */
/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 */


/** Start code to put an i386 or later processor into 32-bit 
 * protected mode. 
 */

#include <arch/cache_ram.h>

.text
.code16
.globl	EXT(_start)
.type EXT(_start), @function

EXT(_start): 
	cli

/* thanks to kmliu@sis.tw.com for this TBL fix ... */
/**/
/* IMMEDIATELY invalidate the translation lookaside buffer before executing*/
/* any further code.  Even though paging is disabled we could still get*/
/*false address translations due to the TLB if we didn't invalidate it.*/
/**/
	xor	%eax, %eax
	mov	%eax, %cr3    /* Invalidate TLB*/

	/* invalidate the cache */
	invd 

	/* Note: gas handles memory addresses in 16 bit code very poorly.
	 * In particular it doesn't appear to have a directive allowing you
	 * associate a section or even an absolute offset with a segment register.
	 *
	 * This means that anything except cs:ip relative offsets are
	 * a real pain in 16 bit mode.  And explains why it is almost
	 * imposible to get gas to do lgdt correctly.
	 *
	 * One way to work around this is to have the linker do the
	 * math instead of the assembler.  This solves the very
	 * pratical problem of being able to write code that can
	 * be relocated.
	 *
	 * An lgdt call before we have memory enabled cannot be 
	 * position independent, as we cannot execute a call
	 * instruction to get our current instruction pointer.
	 * So while this code is relocateable it isn't arbitrarily
	 * relocatable.
	 *
	 * In particular this code can be run with the base address of
	 * the code segment at either 0xffff0000 or 0xf0000.
	 * The first is what is initiallly loaded when the processor 
	 * powers on.  The second is normal real mode segment 0xf000.
	 *
	 * At this point all the linker script does when calculating
	 * gdtptr_offset is return the low 16 bits so your segment
	 * must be 64K aligned.  So it wouldn't be too much work
	 * to support other segments, I just don't see the point
	 * right now.
	 */
	data32	lgdt %cs:EXT(gdtptr_offset)

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	/* Now that we are in protected mode jump to a 32 bit code segment. */
	data32	ljmp	$0x10, $_protected_start


/** This gdt has a 4 Gb code segment at 0x10, and a 4 GB data segment
 * at 0x18; these are Linux-compatible. 
 */


/** GDT. we have modified this from the original freebios to make it 
 * compatible with linux. This puts text at seg 0x10 and data at 0x18
 */
.align	4
.globl EXT(gdtptr)
EXT(gdtptr):
	.word	4*8-1
	.long	gdt		/* we know the offset */

.globl EXT(_estart)
EXT(_estart):

gdt:
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00

	.word	0xffff, (CACHE_RAM_SEG_BASE & 0xffff)		/* flat offset data segment */
	.byte	((CACHE_RAM_SEG_BASE >> 16)& 0xff), 0x93, 0xcf
	.byte	((CACHE_RAM_SEG_BASE >> 24) & 0xff)
	
	.word	0xffff, 0x0000		/* flat code segment */
	.byte	0x00, 0x9b, 0xcf, 0x00	
	
	.word	0xffff, 0x0000		/* flat data segment */
	.byte	0x00, 0x93, 0xcf, 0x00	


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
 */
	.align	4
_protected_start:
	.code32
	intel_chip_post_macro(0x10)	/* post 10 */

	movw	$0x18, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs


