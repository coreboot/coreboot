#include <asm.h>
#include <cpu/p5/macros.h>
#define DOC_WIN_SEG		0xfe00
#define DOC_STACK_SEG		0x0400
#define SPL_RAM_SEG		0x8000

#define DOC_SPL_START_BLK	2	/* 0,1 for IPL */
#define DOC_SPL_SIZE_IN_PAGE	126     /* 1 page = 512 bytes, total 63kB */

/* wow, the usual way to to this hurts. So we do it our way:
 * 32-bit test not needed. 
 */
/* the MCR is 32-bits. You set it, it programs SDRAM. 
 * first check: get Column address size (CAS)
 * Start out assuming that it is 8 bits, then grow. 
 * nicely, if we put the 32-bit MCR value in %ecs, we can 
 * ror it 16 bits, and as we grow the CAS, we just inc cs, and that will
 * set the right value. 
 */
#define INIT_MCR $0xf6641000
#define WRITE_MCR0 movl %ecx, %eax ;  outl %eax, %dx
/*#include <cpu/p5/start32.inc>*/
.org 0xfe000
.code16
_start:	jmp _realstart
gdt:	 //GDT entry 0 (null segment)
	.byte 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
	// GDT 1
	.word	0xffff, 0x0000		/* flat code segment */
	.byte	0x0, 0x9a, 0xcf, 0x0	
	
	.word	0xffff, 0x0000		/* flat data segment */
	.byte	0x0, 0x92, 0xcf, 0x0	

gdtptr:
	.word	3*8-1
	.long	gdt		/* we know the offset */
	.long 0

_realstart:	

	cli
	movw	%cs, %ax
	movw	%ax, %es

		.byte 0x66 /* prefix */
	.byte 0x26
	.byte 0x0f, 0x01, 0x16 /* lgdt [es:offset]*/
	.word  gdtptr -  EXT(_start) /* offset in segment */

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	/* invalidate the cache */
	invd
	data32	ljmp	$0x8, $.Lprotected

.Lprotected:
	.code32
	movw	$0x10, %bx
	movw	%bx, %es
	movw	%bx, %ds
	movw	%bx, %ss
#ifdef USE_BIG_REAL
	andb	$0xfe, %al
	movl	%eax, %cr0
	ljmp $0xfe00, $.Lunprotected
Lunprotected:
code16
#endif

	/* WARNING:	 The new CALLSP code trashes the stack! */
	/* I will leave this here for now, but you should also set it later. */

	xorl	%edi, %edi
	movl	$0x8000006c, %eax
	mov	$0x0cf8,%dx
	outl	%eax,%dx
/*	movb	$0x8000006c, %al
	CALLSP(pci_read_dword)*/
	movb	$0xfc, %dl
	inl	%dx, %eax
	andl    $0xfffc, %ecx
	orl	INIT_MCR,  %ecx
	WRITE_MCR0
	/*
	movb	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
	rorl	$16, %ecx
	/* test 8 bit CAS */
	movb	$0, 0
	movb	$1, 0x800
	cmpb	$0,0
	jnz	sizeram
	inc	%cl
	/* change the value we store each time. It makes debugging easier */
	movb	$2, 0x1000
	cmpb	$0, 0
	jnz	sizeram
	inc	%cl
	movb	$3, 0x2000
	cmpb	$0, 0
	jnz	sizeram
	inc	%cl
	
sizeram:		
	rorl	$16, %ecx
	/* clear the 'no multi page' bit. */
	andw	$0xefff, %cx
	WRITE_MCR0
	/*
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
/* size is now in cx[19:16] */
/* now size the dram */
/* you had best have at least 32M; that's as small as we go */
/* rorr 20 the ecx value, to get row size into lsb */
	movb	$0, 0
	ror	$20, %ecx
	movl	$0x400000, %esi
1:
#ifdef USE_BIG_REAL
	addr32 	movb	$4, %es:(%esi)
#else
 	movb	$4, %es:(%esi)
#endif
	cmpb	$4, 0
	jne	1f
	inc	%cl
	rol	$1, %esi
	jmp	1b
/* size is now in esi */
/* %ecx has setting for register */
1:
	rol	$20, %ecx
/* set 4 banks. */
	orb	$1, %cl
	WRITE_MCR0
	/*
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
/* bank detect */
/* you don't need to even care how large CAS is. 
 * Just set 4-bank mode, and set non-zero numbers into a few places. 
 * if the byte at 0 changes, you have two banks. Trivial.
 */
	movb	$0, 0
	movb	$5, 0x1000
	movb	$6, 0x2000
	movb	$7, 0x4000
	cmpb	$0, 0
	jz	1f /* only one bank */
	orl	$0x80000, %ecx
1: 
	/* clear 4 banks */
	andb	$0xfe, %cl
	WRITE_MCR0
	/*
	mov	$0x6c, %al
	CALLSP(pci_write_dword)
	*/
	/* at this point, dram slot 0 is up.  we hope. */

	/* now for flash ... */
	/* NOTE: MUST NOT DESTROY %ecx! */
	/* INPUT: %al, the register. %ecx, the write data */
	/* Following code courtesy Ollie Lho:	*/
/*
 * ipl.S:	 Initial Program Loader (IPL) for SiS 630 and M-System DoC Millennium
 *
 *
 * Copyright 2000 Silicon Integrated Systems Corporation
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 * Reference:
 *	1. SiS 630 Specification
 *	2. System Management Bus Specification Rev 1.1
 *	3. PC SDRAM Serial Presence Detect (SPD) Specification Rev 1.2B
 *	4. Booting From the DiskOnChip Millennium, M-Systems Application Note Ap-DOC-044
 *	   APR-2000, 93-SR-001-44-7L REV. 1.0
 *
 * $Id$
 */
#define DOC_WIN_SEG		0xfe00
#define DOC_STACK_SEG		0x0400
#define SPL_RAM_SEG		0x8000

#define DOC_SPL_START_BLK	2	/* 0,1 for IPL */
#define DOC_SPL_SIZE_IN_PAGE	126     /* 1 page = 512 bytes, total 63kB */

ipl_start:
#if 0
	/* stack and es already set. */
	/* O.K. we have DRAM now, so set up STACK for CALL/RET */
	movw	$DOC_STACK_SEG, %ax
	movw	%ax, %ss
	movw	$SPL_RAM_SEG, %ax
	movw	%ax, %es
	xorl	%sp, %sp			# clear %sp
#endif
	/* NOTE: in ollie's original code, %ds was at 0xfe00, i.e. 
	 * 0xfe000, i.e. base of DoC. We have to adjust for that when 
	 * we set %edi and %esi
	 */
	movl	$0x4000, %esp			# set %sp
	xorl	%edx, %edx			# clear %dx, start of RAM
	movl	$0xfe800, %esi			# point %si to CDSN Data area
	movl	$0xff000, %edi			# point %di to CDSN Control area
	movl	$DOC_SPL_START_BLK, %ebp	# start page of LinuxBIOS

	movb	$0x84, %al			# Reset DOC Millennium
	call	doc_reset

	movb	$0x85, %al			# Turn into Normal Mode
	call	doc_reset
	
read_next_page:
	movl	$0xff000, %edi			# point %di to CDSN Control area

flash_command:
	movb	$0x03, 0x04(%edi)		# start command cycle
	movb	$0x00, (%esi)			# issue flash command Read00
	call	doc_cycle_end
	
	movw	%bp, %bx			# %bp is current page number

flash_address:
	shll	$0x08, %ebx
        movb	$0x03, %cl			# this one is DANGEROUS but I am
						# sure the upper 3 bytes == 0x00
	
	movb	$0x05, 0x04(%edi)		# start address cycle
0:
	movb	%bl, (%esi)			# write address to CDSNIO
	shrw	$0x08, %bx			# shift next address byte
	loop	0b

	call	doc_cycle_end

wait_for_flash_ready:
	/* delay by reding NOP register before polling the FLASH READY bit,
	   this is inlined to save a call/ret pair */
doc_delay:
	movb	$0x04, %cl			# this one is DANGEROUS but I am
						# sure the upper 3 bytes == 0x00
0:
	movb	0x20(%edi), %al			# read DOC NOP retisger
	loop	0b				# four times
	
	testb	$0x80, 0x04(%edi)		# is flash ready ?
	jz	wait_for_flash_ready

	movb	0x1d(%edi), %al			# init read pipeline
	movw	$0x100, %cx			# 1 page = 512 bytes
	movl	$0xfe800, %esi			# point %si to CDSN Data area
	movl	%edx, %edi			# restore saved current destination
	rep
	movsw

	movl	%edi, %edx			# save current destination
	incw	%bp				# increse current page number
	cmpw	$128, %bp			# moved 63 KB ??
	jl	read_next_page			# no, read next page

sis630ipl_end:
	jmp	spl_vector			# jump to SPL vector

doc_reset:
        /* Input: AL = value write to DOC_CONTROL register
           Clobberd: CX */
        movb    %al, 0x02(%edi)                  # write DoC Control retister
        movb    %al, 0x02(%edi)                  # twice
        ret                                     # End of doc_reset

doc_cycle_end:
        movb    $0x00, 0x1e(%edi)                # flush write pepeline
        movb    $0x01, 0x04(%edi)                # end command cycle
        ret


/* we don't need these. We only do I/O to MCR 0 */
#if 0
pci_write_dword:
	mov	$0,%ah
	mov	$0x80000000,%edx
	or	%ax,%dx
	mov	%edx,%eax
	mov	$0x0cf8,%dx
	outl	%eax,%dx
	mov	$0x0fc,%dl
	mov	%ecx, %eax
	outl	%eax,%dx
	RETSP
pci_read_dword:
	mov	$0,%ah
	mov	$0x80000000,%edx
	or	%ax,%dx
	mov	%edx,%eax
	mov	$0x0cf8,%dx
	outl	%eax,%dx
	mov	$0x0fc,%dl
	inl	%dx, %eax
	mov	%eax, %ecx
	RETSP
#endif

	.org 0xfe1f0
reset_vector:
	.byte	0xea				# jmp to fe00:0000, where IPL
	.word	0x0000, DOC_WIN_SEG		# starts in DoC

spl_vector:
	.byte	0xea				# jmp to 8000:0000, where SPL
	.word	0x0000, SPL_RAM_SEG		# (LinuxBIOS) starts in RAM
pad: 	.byte 0, 0, 0, 0, 0, 0

