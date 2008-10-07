/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2005 Eswar Nallusamy, LANL
 * Copyright (C) 2005 Tyan
 * (Written by Yinghai Lu <yhlu@tyan.com> for Tyan)
 * Copyright (C) 2007 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

/* Init code - Switch CPU to protected mode and enable Cache-as-Ram (CAR). */

#include <macros.h>
#include <amd_geodelx.h>

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

	/* From now on we are 32bit. */
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

	/* selgdt 0x18, flat code segment for CAR */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xcf, 0x00

	/* selgdt 0x20, flat data segment for CAR */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00
gdt_end:

	/* When we come here we are in protected mode. We expand the stack
	 * and copy the data segment from ROM to the memory.
	 *
	 * After that, we call the chipset bootstrap routine that
	 * does what is left of the chipset initialization.
	 *
	 * NOTE: Aligned to 4 so that we are sure that the prefetch
	 * cache will be reloaded.
	 */

	.align  4
	.globl protected_stage0
protected_stage0:
	/* This code was used by v2. TODO. */
	lgdt	%cs:gdtptr
	ljmp	$ROM_CODE_SEG, $__protected_stage0

.globl __protected_stage0
__protected_stage0:
	/* Save the BIST result. */
	movl	%eax, %ebp

	port80_post(0x01)

	movw	$ROM_DATA_SEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* Restore the BIST value to %eax. */
	movl	%ebp, %eax

	.align 4
	/* Here begins CAR support. */
	/* This particular code is straight from coreboot v2. */

	/* DCacheSetup: Setup data cache for use as RAM for a stack. */
DCacheSetup:
	invd

	/* Set cache properties. */
	movl	$CPU_RCONF_DEFAULT, %ecx
	rdmsr

	/* 1MB system memory in write back 1|00100|00. */
	movl	$0x010010000, %eax
	wrmsr

	/* In LX DCDIS is set after POR which disables the cache..., clear
	 * this bit.
	 */
	movl	$CPU_DM_CONFIG0,%ecx
	rdmsr

	/* TODO: Make consistent with i$ init, either whole reg = 0, or just
	 * this bit...
	 */
	andl	$(~(DM_CONFIG0_LOWER_DCDIS_SET)), %eax
	wrmsr

	/* Get cache timing params from BIOS config data locations and apply. */
	/* Fix delay controls for DM and IM arrays. */
	movl	$CPU_BC_MSS_ARRAY_CTL0, %ecx
	xorl	%edx, %edx
	movl	$0x2814D352, %eax
	wrmsr

	movl	$CPU_BC_MSS_ARRAY_CTL1, %ecx
	xorl	%edx, %edx
	movl	$0x1068334D, %eax
	wrmsr

	movl	$CPU_BC_MSS_ARRAY_CTL2, %ecx
	movl	$0x00000106, %edx
	movl	$0x83104104, %eax
	wrmsr

	movl	$GLCP_FIFOCTL, %ecx
	rdmsr
	movl	$0x00000005, %edx
	wrmsr

	/* Enable setting. */
	movl	$CPU_BC_MSS_ARRAY_CTL_ENA, %ecx
	xorl	%edx, %edx
	movl	$0x01, %eax
	wrmsr

	/* Get cleaned up. */
	xorl	%edi, %edi
	xorl	%esi, %esi
	xorl	%ebp, %ebp

	/* DCache Ways0 through Ways7 will be tagged for
	 * LX_STACK_BASE + DCACHE_RAM_SIZE for holding stack.
	 */
	/* Remember, there is NO stack yet... */

	/* Tell cache we want to fill Way 0 starting at the top. */
	xorl	%edx, %edx
	xorl	%eax, %eax
	movl	$CPU_DC_INDEX, %ecx
	wrmsr

	/* Start address for tag of Way0: ebp will hold the incrementing
	 * address. Don't destroy!
	 */
	movl	$LX_STACK_BASE, %ebp	/* Init to start address. */

	/* Set valid bit and tag for this Way (B[31:12]: Cache tag value for
	 * line/way curr. selected by CPU_DC_INDEX.
	 */
	orl	$1, %ebp

	/* Start tag Ways 0 with 128 lines with 32 bytes each: edi will hold
	 * the line counter. Don't destroy!
	 */
	movl	$LX_NUM_CACHELINES, %edi

DCacheSetupFillWay:
	/* Fill with dummy data: zero it so we can tell it from PCI memory
	 * space (returns FFs).
	 */
	/* We will now store a line (32 bytes = 4 x 8 bytes = 4 quad-words). */
	movw	$0x04, %si
	xorl	%edx, %edx
	xorl	%eax, %eax
	movl	$CPU_DC_DATA, %ecx

DCacheSetup_quadWordLoop:
	wrmsr
	decw	%si
	jnz	DCacheSetup_quadWordLoop

	/* Set the tag for this line, need to do this for every new cache
	 * line to validate it!
	 */
	/* Accessing CPU_DC_TAG_I makes the LINE field in CPU_DC_INDEX
	 * increment and thus continue in the next cache line...
	 */
	xorl	%edx, %edx
	movl	%ebp, %eax
	movl	$CPU_DC_TAG, %ecx
	wrmsr

	/* Switch to next line. Lines are in bits 10:4. */
	/* When index is crossing 0x7F -> 0x80 writing a RSVD bit as 0x80 is
	 * not a valid CL anymore!
	 */
	movl	$CPU_DC_INDEX, %ecx
	rdmsr

	/* TODO: Probably would be more elegant to calculate this from
	 * counter var edi...
	 */
	addl	$0x010, %eax
	wrmsr

	decl	%edi
	jnz	DCacheSetupFillWay

	/* 1 Way has been filled, forward start address for next Way,
	 * terminate if we have reached end of desired address range.
	 */
	addl	$LX_CACHEWAY_SIZE, %ebp
	cmpl	$LX_STACK_END, %ebp
	jge	leave_DCacheSetup
	movl	$LX_NUM_CACHELINES, %edi

	/* Switch to next way. */
	movl	$CPU_DC_INDEX, %ecx
	rdmsr
	addl	$0x01, %eax

	/* Let's be sure: reset line index bits 10:4. */
	andl	$0xFFFFF80F, %eax
	wrmsr

	jmp	DCacheSetupFillWay

leave_DCacheSetup:
	xorl	%edi, %edi
	xorl	%esi, %esi
	xorl	%ebp, %ebp

	/* Disable the cache, but... DO NOT INVALIDATE the tags. */
	/* Memory reads and writes will all hit in the cache. */
	/* Cache updates and memory write-backs will not occur! */
	movl	%cr0, %eax
	orl	$(CR0_CD + CR0_NW), %eax	/* Set the CD and NW bits. */
	movl	%eax, %cr0

	/* Now point sp to the cached stack. */
	/* The stack will be fully functional at this location. No system
	 * memory is required at all!
	 */
	/* Set up the stack pointer. */
	movl	$LX_STACK_END, %eax
	movl	%eax, %esp

	/* Test the stack. */
	movl	$0x0F0F05A5A, %edx
	pushl	%edx
	popl	%ecx
	cmpl	%ecx, %edx
	je	DCacheSetupGood
	movb	$0xC5, %al
	outb	%al, $0x80

DCacheSetupBad:
	hlt		/* Issues */
	jmp DCacheSetupBad

DCacheSetupGood:
	/* At this point, CAR should be working. */
	movl	$(LX_STACK_END), %eax
	movl	%eax, %esp

	/* Load a different set of data segments. */
	movw	$CACHE_RAM_DATA_SEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss

lout:
	/* Store zero for the pointer to the global variables. */
	movl    $0, %eax
	pushl   %eax

	/* Store zero for the unused init_detected parameter. */
	pushl   %eax

	/* Restore the BIST result. */
	movl	%ebp, %eax

	/* We need to set ebp? No need. */
	movl	%esp, %ebp
	pushl	%eax		/* BIST */
	jmp	stage1_main
	/* We will not go back. */

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
.byte	0

/* Date? ID string? We might want to put something else in here. */
.ascii DATE

/* Checksum. */
/* .word 0 */
