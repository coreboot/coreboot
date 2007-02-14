## Note: this file contains _multiple_ copyright/license headers, see below.

## 
## This file is part of the LinuxBIOS project.
## 
## Copyright (C) 2000,2007 Ronald G. Minnich <rminnich@gmail.com>
##
## Copyright (C) 2007 coresystems GmbH
## Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; version 2 of the License.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
## 

# init code - switch cpu to pmode and enable cache as ram.

#include "macros.h"

#define ROM_CODE_SEG 0x08
#define ROM_DATA_SEG 0x10

#define CACHE_RAM_CODE_SEG 0x18
#define CACHE_RAM_DATA_SEG 0x20
#	.section ".rom.text"

	.code16
#.=0
	.globl _stage0
#	.section ".rom.text"
_stage0:
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
	/* NOTE: WIth GNU assembler version 2.15.94.0.2.2 (i386-redhat-linux) using BFD version 2.15.94.0.2.2 20041220
	 * this works fine without all the ld hackery and other things. So leave it as is with this comment. 
	 */

#	movw	%cs, %ax
#	shlw	$4, %ax
#	movl	$0xffffe000, %ebx
#	movw	$gdt16 + 0xe000, %bx
	#subw	%ax, %bx
	#data32	lgdt %cs:(%bx)
	data32	lgdt %cs:gdtptr

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	/* Restore BIST result */
	movl	%ebp, %eax


	// port80_post (0x23)     /* post 0x01 */
	/* Now we are in protected mode. Jump to a 32 bit code segment. */
	data32 ljmp    $ROM_CODE_SEG, $protected_stage0
	/* I am leaving this weird jump in here in the event that future gas bugs force it to be used. */
	#.byte 0x66
	.code32
	#ljmp    $ROM_CODE_SEG, $protected_stage0

	#.code16
	.align  4
	.globl gdt16
gdt16 = . - _stage0 
gdt16x:
	.word   gdt16xend - gdt16x -1 /* compute the table limit */
	.long	gdt16x
	.word   0

	/* selgdt 0x08, flat code segment */
	.word   0xffff, 0x0000
	.byte   0x00, 0x9b, 0xcf, 0x00

	/* selgdt 0x10,flat data segment */
	.word   0xffff, 0x0000
	.byte   0x00, 0x93, 0xcf, 0x00
gdt16xend:

	/* From now on we are 32bit */

	.code32

/* We have two gdts where we could have one. That is ok. 
  * Let's not worry about this -- optimizing gdt is pointless since we're only in it for a little bit. 
  * BTW note the trick below: the gdt points to ITSELF, and the first good descriptor is at offset 8. 
  * so you word-align the table, and then because you chose 8, 
  * you get a nice 64-bit aligned gdt endtry, which is 
  * good as this is the size of the entry. Just in case you ever wonder why people do this. 
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

	/* selgdt 0x18, flat code segment for CAR */
	.word   0xffff, 0x0000
	.byte   0x00, 0x9b, 0xcf, 0x00

	/* selgdt 0x20,flat data segment for CAR */
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
	.globl protected_stage0
protected_stage0:
	//This code was used by v2. TODO
	lgdt    %cs:gdtptr
	ljmp    $ROM_CODE_SEG, $__protected_stage0

.globl __protected_stage0
__protected_stage0:
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

.align 4

## 
## This file is part of the LinuxBIOS project.
## 
## Copyright (C) 2005 Eswar Nallusamy, LANL
##
## Copyright (C) 2005 Tyan
## Written by Yinghai Lu <yhlu@tyan.com> for Tyan.
##
## Copyright (C) 2007 coresystems GmbH
## Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
## 

/* We will use 4Kbytes only for cache as ram. This is
 * enough to fit in our stack.
 * 
 * disable HyperThreading is done by eswar
 * the other is very similar to the AMD CAR, except remove amd specific msr 
 */

#ifndef CONFIG_CARSIZE
#define CacheSize 4096
#else
#define CacheSize CONFIG_CARSIZE
#endif

#ifndef CONFIG_CARBASE
#define CacheBase (0xd0000 - CacheSize) 
#else
#define CacheBase CONFIG_CARBASE
#endif

#define ASSEMBLY
#include "mtrr.h"

	/* Save the BIST result */
	movl    %eax, %ebp

CacheAsRam:
        /* Check whether the processor has HT capability */
        movl	$01, %eax
        cpuid
        btl	$28, %edx
        jnc	NotHtProcessor
        bswapl	%ebx
        cmpb	$01, %bh
        jbe	NotHtProcessor

        /* It is a HT processor; Send SIPI to the other logical processor
         * within this processor so that the CAR related common system 
	 * registers are programmed accordingly
	 */

        /* Use some register that is common to both logical processors
         * as semaphore. Refer Appendix B, Vol.3
	 */

        xorl	%eax, %eax
        xorl	%edx, %edx
        movl	$0x250, %ecx
        wrmsr

        /* Figure out the logical AP's APIC ID; the following logic will work
         * only for processors with 2 threads.
	 *
         * Refer to Vol 3. Table 7-1 for details about this logic
	 */
        movl	$0xFEE00020, %esi
        movl	(%esi), %ebx
        andl	$0xFF000000, %ebx
        bswapl	%ebx
        btl	$0, %ebx
        jnc	LogicalAP0
        andb	$0xFE, %bl
        jmp	SendSIPI
LogicalAP0:
        orb	$0x01, %bl
SendSIPI:
        bswapl	%ebx  /* ebx - logical AP's APIC ID */

        /* Fill up the IPI command registers in the Local APIC mapped to 
	 * default address and issue SIPI to the other logical processor 
	 * within this processor die.
	 */

RetrySIPI:
        movl	%ebx, %eax
        movl	$0xFEE00310, %esi
        movl	%eax, (%esi)

        /* SIPI vector - F900:0000 */
        movl	$0x000006F9, %eax
        movl	$0xFEE00300, %esi
        movl	%eax, (%esi)

        movl	$0x30, %ecx
SIPIDelay:
        pause
        decl	%ecx
        jnz	SIPIDelay

        movl	(%esi), %eax
        andl	$0x00001000, %eax
        jnz	RetrySIPI

        /* Wait for the Logical AP to complete initialization */
LogicalAPSIPINotdone:
        movl	$0x250, %ecx
        rdmsr
        orl	%eax, %eax
        jz	LogicalAPSIPINotdone



NotHtProcessor:
        /* Set the default memory type and enable fixed and variable MTRRs */
        movl	$MTRRdefType_MSR, %ecx
        xorl	%edx, %edx
        /* Enable Variable and Fixed MTRRs */
        movl	$0x00000c00, %eax
        wrmsr

	/*Clear all MTRRs */
	xorl	%edx, %edx
	movl	$fixed_mtrr_msr, %esi
clear_fixed_var_mtrr:
        lodsl	(%esi), %eax
        testl	%eax, %eax
        jz	clear_fixed_var_mtrr_out

        movl	%eax, %ecx
        xorl	%eax, %eax
        wrmsr   

        jmp     clear_fixed_var_mtrr
clear_fixed_var_mtrr_out:

#if CacheSize == 0x10000 
        /* enable caching for 64K using fixed mtrr */
        movl    $0x268, %ecx  /* fix4k_c0000*/
        movl    $0x06060606, %eax /* WB IO type */
	movl    %eax, %edx
        wrmsr
	movl	$0x269, %ecx
	wrmsr
#endif

#if CacheSize == 0x8000
        /* enable caching for 32K using fixed mtrr */
        movl    $0x269, %ecx  /* fix4k_c8000*/
        movl    $0x06060606, %eax /* WB IO type */
	movl    %eax, %edx
	wrmsr
#endif

        /* enable caching for 16K/8K/4K using fixed mtrr */
        movl    $0x269, %ecx  /* fix4k_cc000*/
#if CacheSize == 0x4000
        movl    $0x06060606, %edx /* WB IO type */
#endif
#if CacheSize == 0x2000
        movl    $0x06060000, %edx /* WB IO type */
#endif
#if CacheSize == 0x1000
        movl    $0x06000000, %edx /* WB IO type */
#endif
	xorl    %eax, %eax
	wrmsr

#if defined(CONFIG_XIP_ROM_SIZE) && defined(CONFIG_XIP_ROM_BASE)
        /* enable write base caching so we can do execute in place
         * on the flash rom.
         */
        movl    $0x202, %ecx
        xorl    %edx, %edx
        movl    $(XIP_ROM_BASE | MTRR_TYPE_WRBACK), %eax
        wrmsr

        movl    $0x203, %ecx
        movl    $0x0000000f, %edx
        movl    $(~(XIP_ROM_SIZE - 1) | 0x800), %eax
        wrmsr
#endif /* XIP_ROM_SIZE && XIP_ROM_BASE */

        /* enable cache */
        movl    %cr0, %eax
        andl    $0x9fffffff,%eax
        movl    %eax, %cr0

	/* Read the range with lodsl*/
        movl    $CacheBase, %esi
	cld
        movl    $(CacheSize>>2), %ecx
        rep     lodsl

	/* Clear the range */
        movl    $CacheBase, %edi
        movl    $(CacheSize>>2), %ecx
        xorl    %eax, %eax
        rep     stosl


	/* TODO: make this a config variable */
#if CONFIG_CARTEST
	/* check the cache as ram */
        movl  $CacheBase, %esi
        movl    $(CacheSize>>2), %ecx
.xin1:  
        movl  %esi, %eax
        movl  %eax, (%esi)
        decl  %ecx
        je      .xout1
        add     $4, %esi
        jmp     .xin1
.xout1: 

        movl  $CacheBase, %esi
//        movl    $(CacheSize>>2), %ecx
	movl $4, %ecx
.xin1x:
        movl  %esi, %eax

        movl    $0x4000, %edx
        movb    %ah, %al 
.testx1:  
        outb %al, $0x80
        decl    %edx
        jnz .testx1
        
        movl  (%esi), %eax
        cmpb 0xff, %al
        je .xin2  /* dont show */

        movl    $0x4000, %edx
.testx2:
        outb %al, $0x80
        decl    %edx
        jnz .testx2
        
.xin2:  decl     %ecx
        je      .xout1x
        add     $4, %esi
        jmp     .xin1x
.xout1x:

#endif

	movl	$(CacheBase+CacheSize-4), %eax
	movl    %eax, %esp

	/* Load a different set of data segments */
	movw    $CACHE_RAM_DATA_SEG, %ax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %ss

lout:

	/* Restore the BIST result */
	movl	%ebp, %eax
	/* We need to set ebp ? No need */
	movl	%esp, %ebp
	pushl 	%eax  /* bist */
	call    stage1_main
	/* We will not go back */
fixed_mtrr_msr: 
        .long   0x250, 0x258, 0x259
        .long   0x268, 0x269, 0x26A
        .long   0x26B, 0x26C, 0x26D
        .long   0x26E, 0x26F
var_mtrr_msr:   
        .long   0x200, 0x201, 0x202, 0x203
        .long   0x204, 0x205, 0x206, 0x207
        .long   0x208, 0x209, 0x20A, 0x20B
        .long   0x20C, 0x20D, 0x20E, 0x20F
        .long   0x000 /* NULL, end of table */




## 
## This file is part of the LinuxBIOS project.
## 
## Copyright (C) 2007 coresystems GmbH
## Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
## 

# reset vector

# RVECTOR: size of reset vector, default is 0x10
# RESRVED: size of vpd code, default is 0xf0
#
# BOOTBLK: size of bootblock code, default is 0x1f00 (8k-256b)
#

SEGMENT_SIZE = 0x10000
RVECTOR      = 0x00010
#.=0x1ff0
#.=0xfffffff0
#.=_stage0 + 0x1ff0
# due to YET ANOTHER BUG in GNU  bintools, you  can NOT have a code16 here. 
# I think we should leave it this way forever, as the bugs come and go -- and come again
#	.code16
#	.section ".rom.text"
.section ".reset", "ax"
	.globl _resetjump
_resetjump:
	/* gnu bintools win again. this jumps to stage0 - 2. Sigh. */
#	jmp _stage0
	.byte  0xe9
	.int   _stage0 - ( . + 2 )
	/* Note: The above jump is hand coded to work around bugs in binutils.
	 * 5 byte are used for a 3 byte instruction.  This works because x86
	 * is little endian and allows us to use supported 32bit relocations
	 * instead of the weird 16 bit relocations that binutils does not
	 * handle consistenly between versions because they are used so rarely.
	 */	
.byte	0

# date? idstring? we might want to put something else in here.
.ascii DATE

# checksum
#.word  0
