#include "asm.h"
//#include <cpu/p5/macros.h>
#include "intel.h"
/*
 */

#define DOC_WIN_SEG		0xfe00
#define DOC_STACK_SEG		0x0400
#define SPL_RAM_SEG		0x8000

#define DOC_SPL_START_BLK	2	/* 0,1 for IPL */
#define DOC_SPL_SIZE_IN_PAGE	252     /* 1 page = 512 bytes, total 63kB */

/* wow, the usual way to to this hurts. So we do it our way:
 * 32-bit test not needed. 
 */
/*#include <cpu/p5/start32.inc>*/
.org 0x0
.code16
_start:	jmp _realstart


/*  initialize registers */
#define REG(a) a&0xfc, (a&0x3)|4
register_table:
	.byte REG(0x78), 0x01
	.byte REG(0x68), 0x00
	.byte REG(0x6b), 0x00
	.byte REG(0x58), 0x88
	.byte REG(0x59), 0x88
	.byte REG(0x5a), 0x08
	.byte REG(0x5b), 0x10
	.byte REG(0x5c), 0x10
	.byte REG(0x5d), 0x10
	.byte REG(0x5e), 0x10
	.byte REG(0x5f), 0x10
	.byte REG(0x56), 0x10
	.byte REG(0x57), 0x10
	.byte REG(0x60), 0xff
	.byte REG(0x64), 0xee
	.byte REG(0x65), 0xee
	.byte REG(0x66), 0xee
	.byte REG(0x69), 0x04
	.byte REG(0x6a), 0x00
	.byte REG(0x6c), 0x00
	.byte REG(0x6d), 0x37
	// This extra set is redundant, but needed below.
	// LEAVE IT HERE. 
	.byte REG(0x6c), 0x0
	.byte 0x0
_realstart:	
//        movb	$0xaa,%al
//	outb	%al,$0x80
	cli
	movw	%cs, %ax
	movw	%ax, %ds

 movl $0x80008840, %eax
 movw  $0x0cf8, %dx
 outl %eax,%dx
 movb $0xfc,%dl
 movb $0x54,%al
 outb %al,%dx	
// inl  %dx, %eax
// orw  $0x7f10, %ax
// outw  %ax,%dx
	
//.code32
#define loop200 $0x5000
#define loop100 $0x2500
movl	$0x80000000,%esp
ram_set_registers:
	movw	$register_table, %si
	cld
1:
	movl	%esp, %eax
	orb	(%si), %al
	testb	%al, %al
	jz	done_ram_set_registers
	andb	$0xf8, %dl
	outl	%eax, %dx
	
	inc	%si
	orb	(%si), %dl
	inc	%si
	movb	(%si), %al
	outb	%al, %dx
//	PCI_WRITE_CONFIG_BYTE
	inc	%si
	jmp	1b
//	movb	$0xbb,%al
//	outb	%al,$0x80
	 	
done_ram_set_registers:
	xorw	%si, %si
//	movl	$0x4000000, %edi
/* begin to initialize*/
	// I forget why we need this, but we do
	movl $0xa55a5aa5, %ecx
	movl %ecx, (%si)
	
//	mov %ecx, (%edi)
// REMEMBER:	 0xcf8 was set to 0x8000006c above. No need to set 
// again!
// Also, remember:	%edx has 0xcfc from the above loop!
//                      that is the correct value!
/* set NOP*/
#define WRITE_6c(unused,val) movb	val, %al ; outb 	%al, %dx
	WRITE_6c($0x6C, $0x01)

/* wait 200us*/
	// You need to do the memory reference. That causes the nop cycle. 
	movw	(%si), %cx
//	mov	(%edi), %ecx
	DELAY(loop200)

/* set precharge */
	WRITE_6c($0x6C, $0x02)

/* dummy reads*/
	movw	(%si), %cx
//	mov	(%edi), %ecx

/* set CBR*/
	WRITE_6c($0x6C, $0x04)
	
/* do 8 reads and wait 100us between each - from via*/

	movb	$8, %bl
eightreads:	
	movw	(%si), %cx
//	mov	(%edi), %ecx
	DELAY(loop100)
	dec	%bl
	cmpb	$0, %bl
	ja	eightreads

/* set MRS*/
	// 0x150 is cas2. We are now using 0x1d0, which is cas3
	WRITE_6c($0x6c, $0x03)

	movw $0x1d0, %di
	movl (%di), %ecx
//	movl $0x40001d0, %ecx
//	movl (%ecx), %ecx

/* set to normal mode */
	WRITE_6c($0x6C, $0x00)

	movl $0x55aa55aa, %ecx
	movl %ecx, (%si)
	movl (%si), %ecx

	// Set the refresh rate. 
	movb	$0x68, %al
	movb	$0xf8, %dl
	outl	%eax, %dx
	addw	$7, %dx
	movb	$0x65, %al
	outb	%al, %dx
	//CS_WRITE($0x6A, $0x65)
	// enable multi-page open
	inc	%dl
	movb	$1, %al
	outb	%al, %dx
	//CS_WRITE($0x6B, $0x01)
//	movb	$0xcc,%al
//	outb	%al,$0x80
		
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
// .code16
#define DOC_WIN_SEG		0xfe00
#define DOC_STACK		0x04000
#define SPL_RAM		0x80000

#define DOC_SPL_START_BLK	2	/* 0,1 for IPL */
#define DOC_SPL_SIZE_IN_PAGE	252     /* 1 page = 512 bytes, total 63kB */

ipl_start:
//#if 0
	/* stack and es already set. */
	/* O.K. we have DRAM now, so set up STACK for CALL/RET */
	movw	$DOC_STACK_SEG, %ax
	movw	%ax, %ss
	movw	$SPL_RAM_SEG, %ax
	movw	%ax, %es
//	movl	$DOC_STACK_SEG, %esp			# set %sp
//#endif
	/* NOTE: in ollie's original code, %ds was at 0xfe00, i.e. 
	 * 0xfe000, i.e. base of DoC. We have to adjust for that when 
	 * we set %edi and %esi
	 */
//	movl	$0x80008840,%eax
//	mov	$0xcf8,%dx
//	outl	%eax,%dx
//	mov	$0xfc,%dl
//	inl	%dx,%eax
	movb	$0xbb, %al
	outb	%al,$0x80
	xorw	%sp,%sp
	xorw	%dx,%dx
//	movl	$DOC_STACK, %esp		# set %sp
//	movl	$SPL_RAM, %edx			# start of RAM
//	xorl	%ecx, %ecx			# clear %ecx
	movw	$0x800, %si			# point %si to CDSN Data area
	movw	$0x1000,%di			# point %di to CDSN Control area
	movw	$DOC_SPL_START_BLK, %bp		# start page of LinuxBIOS
        movb	$0x0, %ds:0x03(%di)
        movb	$0x84,%ds:0x02(%di)
	movb	$0x84,%ds:0x02(%di)
	call	delay4nop
	movb	$0x85,%ds:0x02(%di)
	movb	$0x85,%ds:0x02(%di)
	call	delay4nop
read_next_page:
	movw	$0x1000, %di			# point %di to CDSN Control area
	movw	$0x800, %si
flash_command:
	movb	$0x13, %ds:0x04(%di)		# start command cycle
	call	delay4nop
	movb	$0x00, %ds:(%si)			# issue flash command Read00

 	movb    $0x00, %ds:0x1e(%di)
	movb    $0x11, %ds:0x04(%di)

	movw	%bp, %bx			# %bp is current page number

flash_address:
	shll	$0x08, %ebx
        movw	$0x03, %cx			# this one is DANGEROUS but I am
	
	movb	$0x15, %ds:0x04(%di)		# start address cycle
	call	delay4nop
0:
	movb	%bl, %ds:(%si)			# write address to CDSNIO
	shrw	$0x08, %bx			# shift next address byte
	loop	0b
        movb    $0x00, %ds:0x1e(%di)
	movb    $0x31, %ds:0x04(%di)
//	call	doc_cycle_end
	call	delay4nop
	

wait_for_flash_ready:
	/* delay by reding NOP register before polling the FLASH READY bit,
	   this is inlined to save a call/ret pair */
doc_delay:
	movb	%ds:0x04(%di),%ah
	movw	$0x04, %cx			# this one is DANGEROUS but I am
						# sure the upper 3 bytes == 0x00
0:
	movb	%ds:0x20(%di), %al			# read DOC NOP retisger
	loop	0b				# four times
	
	testb	$0x80,%ah		# is flash ready ?
	jz	wait_for_flash_ready

	movb	%ds:0x1d(%di), %al			# init read pipeline
	movw	$0x1ff, %cx			# 1 page = 512 bytes
	movw	$0x800, %si			# point %si to CDSN Data area
	movw	%dx, %di			# restore saved current destination
       
	rep	movsb
	movw	$0x101f,%si
	movsb
//	call	dely4nop
        movw	%di, %dx			# save current destination
	mov	%bp,%di
	sub	$128,%di
	shl	$9,%di
	movb	%es:(%di),%al
	out	%al,$0x80
//	DELAY(loop200)
//	DELAY(loop200)
//	mov	$0xffff,%cx
//aa:	loop	aa	
	inc	%di
	movb	%es:(%di),%al
	out	%al,$0x80
	incw	%bp				# increse current page number
	cmpw	$128, %bp			# moved 63 KB ??
	jl	read_next_page			# no, read next page
		
sis630ipl_end:
	# jmp to 8000:0000, where SPL
	# (LinuxBIOS) starts in RAM
#	ljmp $0x10,$0x8000
	movb	$0xdd, %al
	outb	%al,$0x80
	jmp	spl_vector

delay4nop:
	movb %ds:0x20(%di),%al
	movb %ds:0x20(%di),%al
	movb %ds:0x20(%di),%al
	movb %ds:0x20(%di),%al
	ret

//doc_reset:
        /* Input: AL = value write to DOC_CONTROL register
           Clobberd: CX */
//        movb    %al, 0x02(%edi)                  # write DoC Control retister
//        movb    %al, 0x02(%edi)                  # twice
//        ret                                     # End of doc_reset

//doc_cycle_end:
//        movb    $0x00, 0x1e(%di)                # flush write pepeline
//        movb    $0x31, 0x04(%di)                # end command cycle
//        ret


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

	.org 0x01f0
reset_vector:
	.byte	0xea				#jump to fe00:0000, where IPL
	.word	0x0000, DOC_WIN_SEG		#starts in DoC

spl_vector:
//	invd
//	movb	$0xdd,%al
//	outb	%al,$0x80
//	jmpl	$0x10, $0x80000
	.byte	0xea
	.word	0x0000, 0x8000
# jumpl	$0x80000
# put this here to buy some room
pad:	.byte 0,0,0,0,0,0
