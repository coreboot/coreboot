/*
 * ipl.S:	 Initial Program Loader (IPL) for M1535D and M-System DoC Millennium
 *
 * MODIFIED BY BHARATH ON 09/12/01
 *	
 * ORIGINAL Reference:
 *	1. SiS 630 Specification
 *	2. System Management Bus Specification Rev 1.1
 *	3. PC SDRAM Serial Presence Detect (SPD) Specification Rev 1.2B
 *	4. Booting From the DiskOnChip Millennium, M-Systems Application Note Ap-DOC-044
 *	   APR-2000, 93-SR-001-44-7L REV. 1.0
 *
 * $Id$
 */

#include "ipl.h"
/*
.org 0xfe000
*/
.code16

ali_start:
/*	Must set regs on North Bridge (device 0) */

	movw	$0x00f9, %cx	/*f9h*/
	movb	$0x08, %al
	CALL_SP(SET_PCI)		
	
	movb	$0x8c,%al	/* 8eh */
	movb	$0xf8,%dl
	outl	%eax,%dx
	movb	$0xfe,%dl
	inb	%dx,%al
	orb	$0x08,%al
#	movb	$0xeb, %al
	outb	%al,%dx			
	

	movb	$0x82, %cl	/* 82h */
	movb	$0x10, %al
	CALL_SP(SET_PCI)

/* CHANGED TO 0x88e3 BY BHARATH FOR M7101 */
	
	movw	$0x88e3, %cx		/* M7101 e3h*/
	movb	$0x50, %al
	CALL_SP(SET_PCI)

/* COMMENTED OUT BY BHARATH - NOT NEEDED FOR M1535D */

/*
	movb	$0x04, %cl
	movb	$0x01, %al
	CALL_SP(SET_PCI)	

	movw	$0x88e0, %ax
	movb	$0xf8, %dl	
	outl    %eax, %dx
	movb	$0xff, %dl
	inb	%dx, %al
	outb	%al, $0x80	cheng 0615 test
	movb	$0x66, %al
	outb	%al, $0x80	
*/
	
/*	
	movb	$0xe2, %cl
	movb	$0x00, %al
	CALL_SP(SET_PCI)
*/

/* UNCOMMENTED BY BHARATH - NEEDED FOR M1535D */

	movb    $0xd1, %cl 	/*1535d d1 reg  and 1543c have not same*/
	movb	$0x46, %al
	CALL_SP(SET_PCI)
	
/* CHANGED TO f0 BY BHARATH */
		
	movb	$0xf0, %cl	/*1535d f0 reg 1543c e0 reg*/
	movb	$0x01, %al
	CALL_SP(SET_PCI)

/* COMMENTED OUT BY BHARATH - NOT NEEDED FOR M1535D */

/*
	movb	$0xe2, %cl	
	movb	$0x20, %al
	CALL_SP(SET_PCI) */

#if 0
/* UNCOMMENTED BY BHARATH - NEEDED FOR M1535D */
/* but we don't have much room. */
		
	movb	$0x80, %cl
	movb	$0x3e, %al
	CALL_SP(SET_PCI)
#endif
/* OK, we need to disable some GPOs (!@#$?) 
   Ron also figured the input in AL should be 0x40*/

        movb    $0xb8,%cl
        movb    $0x40,%al 
        CALL_SP(SET_PCI)

	movw	$0x3847, %cx
	movb	$0x40, %al
	CALL_SP(SET_PCI)
/*--------------------------sizing loop start---------------------*/
			
		movw	$0xa16e, %si			#cheng0607 as table before
sizing_loop_start:
		cli					# Disables the maskable
							# hardware interrupts.
		movw	%cs, %ax			# makes data segment ==
		movw	%ax, %ds			# code segment
	
		movw	%si, %bx		

		movb	$0x02, %bl
		CALL_SP(I2C_read_byte)			
						
		cmpb	$0x04, %al			/*exist dram ?*/
		jnz	sizing_end
	
		addb	$0x02, %bl
		CALL_SP(I2C_read_byte)  
		
		movb	%al, %ah        		/*ah=column address bits*/
		subb	$0x08, %ah			/*convert to data sheet value*/
			
		addb	$0x01, %bl
		CALL_SP(I2C_read_byte)
		
		shlb	$0x04, %ah			/*ah[7,6,5,4]=ca type*/
		movb	%al, %bl			/*bl=al=number of row*/

		movb	$0x0c, %al
		cmpb	$0x01, %bl
		jz	row_ok				/*1 row*/
		movb	$0x0f, %al
	#	cmpb	$0x02, %bl
	#	jz	row_ok				/*2 rows*/
	#	jmp	sizing_end			/*no rows*/
	
row_ok:
		orb	%al, %ah			/*ah=[ca type][number of rows]*/

/*get size*/
		movb	$0x01f, %bl
		CALL_SP(I2C_read_byte)			/*get row size*/
				
		clc					#convert spd value to data sheet value
		movb	$0x00, %cl			#chang on 0607 by cheng
aa:		
		shrb	$0x01, %al
		jc	size_ok
		inc	%cl
		jmp	aa	

size_ok:						
		movb	%cl, %al			/*bit: 15...12  11...........8  7......0 */
							/*ax= [ca type][number of rows][row size]*/
		movw	%ax, %di			/*di= [ca type][number of rows][row size]*/
	
		movw	%si, %cx
		xorb	%ch, %ch			/*cheng0606	movw	$M1631, %cx   */
/*;as up	mov	cl, cs:[Dis_Row_Tbl][si]	/cl=6eh/72h/76h	*/
	
		movw	%di, %bx	
		andb	$0x01, %bh
		shlb	$0x03, %bh
		orb	%bh, %bl			/*bl= bit23-20*/
		movb	%bl, %al			/*al= bit23-20*/
							/*bit: 15.........12   11....4   3....0	*/
		rorw	$0x0c, %di			/*di= [number of rows][row size][ca type]*/
		movw	%di, %bx
		andb	$0x0f, %bl			/*bl=ca type*/
		shlb	$0x04, %al				
		orb	%bl, %al			/*al=reg:6eh/72h/76h  value*/
		CALL_SP(SET_PCI)	
			
		inc	%cl
	/*;;;	ROM_CALL       	Get_PCI	*/
	/*;;;	and	al, 11100000b  */
	/*;;;	or	al, 00010000b			/set sdram	*/	
		movb	$0xf0, %al			/*set bit[31-27]*/
		shrb	$5, %bh
		orb	%bh, %al
	
	#	outb	%al, $0x80	/*-----cheng 0618-----*/

		CALL_SP(SET_PCI)
		jmp	chk_loop

/*if error, go here*/
sizing_end:
		movw	%si, %cx
#cheng0606		movw	$M1631, %cx
		xorb	%ch, %ch
Dis_Next_Row_Data:
		
/*;as up	mov	cl, cs:[Dis_Row_Tbl][si]*/
#cheng 0615		movb	$0x00, %al		
#cheng 0615		CALL_SP(SET_PCI)
		
		addb	$0x01, %cl            
		
	#	movb	%cl, %al	/*cheng 0621*/
	#	outb	%al, $0x80	/*cheng 0621*/

		movb	$0xe0, %al   
		CALL_SP(SET_PCI)	

chk_loop:
	#	movw	%si, %ax
	#	outb	%al, $0x80	#cheng0608 for test	
	
		addw	$0x0204, %si
		cmpw	$0xa77a, %si
		jnz	sizing_loop_start	
		
	#	movw	%si, %ax
	#	outb 	%al, $0x80		
		movb	$0x7e, %cl
		movb	$0x2f, %al	/*cheng 0628 ago is 28h*/
		CALL_SP(SET_PCI)	/*cheng 0617 refresh on */

		jmp    ipl_start
	#	nop
		
/*cheng0605 start------------------------------------------------*/

SET_PCI:
	#	movb 	$0x22, %al
	#	outb	%al, $0x80		#cheng0611		

		rorl	$16, %ecx		
		movw	%ax, %cx		
		rorl	$16, %ecx		
		movl	$0x80000000, %eax	
		movw	%cx, %ax
		andb	$0x0fc, %al 
	
		movw	$0x0cf8, %dx		/*index is 0cf8h*/
		outl	%eax, %dx
		addb	$4, %dl			/*start from 0cfch*/
		movb	%cl, %al
		andb	$0x03, %al
		addb	%al, %dl	  	/*byte index to read*/
		rorl	$16, %ecx		
		movw	%cx, %ax		
		rorl	$16, %ecx		
		outb	%al, %dx    
		RET_SP

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
/*
.code32
 */
ipl_start:
	#	movb	$0x55, %al
	#	outb	%al, $0x80	/* cheng 0617 test */

/*#if 0*/
	/* stack and es already set. */
	/* O.K. we have DRAM now, so set up STACK for CALL/RET */
	movw	$DOC_STACK_SEG, %ax
	movw	%ax, %ss
	movw	$SPL_RAM_SEG, %ax
	movw	%ax, %es
/*	movl	$DOC_STACK_SEG, %esp			# set %sp */
/*#endif*/
	/* NOTE: in ollie's original code, %ds was at 0xfe00, i.e. 
	 * 0xfe000, i.e. base of DoC. We have to adjust for that when 
	 * we set %edi and %si
	 */
#	movl	$DOC_STACK, %esp		# set %sp
#	movl	$SPL_RAM, %edx			# start of RAM
#	xorl	%ecx, %ecx			# clear %ecx
	xorw	%sp, %sp	/*cheng 0619*/
	xorw	%dx, %dx	/*cheng 0619*/

	movw	$0x800, %si			# point %si to CDSN Data area
	movw	$0x1000, %di			# point %di to CDSN Control area
	movw	$DOC_SPL_START_BLK, %bp		# start page of LinuxBIOS

	movb	$0x84, %al			# Reset DOC Millennium
	call	doc_reset

	movb	$0x85, %al			# Turn into Normal Mode
	call	doc_reset
	
read_next_page:
	movw	$0x1000, %di			# point %di to CDSN Control area

flash_command:
	movb	$0x03, 0x04(%di)		# start command cycle
	movb	$0x00, (%si)			# issue flash command Read00
	call	doc_cycle_end
	
	movw	%bp, %bx			# %bp is current page number

flash_address:
	shll	$0x08, %ebx
        movb	$0x03, %cl			# this one is DANGEROUS but I am
						# sure the upper 3 bytes == 0x00
	
	movb	$0x05, 0x04(%di)		# start address cycle
0:
	movb	%bl, (%si)			# write address to CDSNIO
	shrw	$0x08, %bx			# shift next address byte
	loop	0b

	call	doc_cycle_end

wait_for_flash_ready:
	/* delay by reding NOP register before polling the FLASH READY bit,
	   this is inlined to save a call/ret pair */
doc_delay:
	movb	$0x04, %cl			# this one is DANGEROUS but I am
						# sure the upper 3 bytes == 0x00
#	movb	$0x33, %al
#	outb	%al, $0x80	/*0618 test*/

0:
	movb	0x20(%di), %al			# read DOC NOP retisger
	loop	0b				# four times
#	outb	%al, $0x80	/*0618 test*/
	
	testb	$0x80, 0x04(%di)		# is flash ready ?
	jz	wait_for_flash_ready

#	movb	$0x33, %al
#	outb	%al, $0x80	/*---cheng 0618 test---*/

	movb	0x1d(%di), %al			# init read pipeline
	movw	$0x100, %cx			# 1 page = 512 bytes
	movw	$0x800, %si			# point %si to CDSN Data area
	movw	%dx, %di			# restore saved current destination
	rep
	movsw

	movw	%di, %dx			# save current destination
	incw	%bp				# increse current page number
	cmpw	$128, %bp			# moved 63 KB ??
	jl	read_next_page			# no, read next page

sis630ipl_end:
	# jmp to 8000:0000, where SPL
	# (LinuxBIOS) starts in RAM
#	ljmp $0x10,$0x8000
	
#	movb	$0x66, %al
#	outb	%al, $0x80	/*0618 test*/

	jmp	spl_vector	
	
/* ------------------------------------------------------------------------------------------*/
doc_reset:
        /* Input: AL = value write to DOC_CONTROL register
           Clobberd: CX */
        movb    %al, 0x02(%di)                  # write DoC Control retister
        movb    %al, 0x02(%di)                  # twice
        ret                                     # End of doc_reset

doc_cycle_end:
        movb    $0x00, 0x1e(%di)                # flush write pepeline
        movb    $0x01, 0x04(%di)                # end command cycle
        ret

		
/*---------------------------------------table--------------------*/
/*		dimm      0      1       2   */
/*
dimm_tbl:
		.byte    0x0a1, 0x0a3, 0x0a5
size_tbl:
		.byte    0x01,  0x02,  0x04,  0x08,  0x10,  0x20,  0x40,  0x80
                .byte    0x00,  0x01,  0x02,  0x03,  0x04,  0x05,  0x06,  0x07
Dis_Row_Tbl:
		.byte    0x06e,  0x072,  0x076
*/
/*
;=============================================		
;                                            =
;	Read SPD value for Dram sizing	     =  CHENG 3.7.2001
;					     =	
;   INPUT    bl := offset to read            =
;            bh := eeprom ID                 =                                       
;                                            =                                       
;   OUTPUT   al := data                      =                                       
;            carry clear = read successful   =                                       
;            carry set   = read failed       =                                      
;                                            =                                       
;   CHANGES REGISTERS: cx, dx, al            =
;					     =	
;=============================================	
*/
I2C_read_byte:
		movw	$0x5000, %dx
		movb	$0x0ff, %al
		outb	%al, %dx		#clear status
	#	movb	$0x77, %al	/*cheng 0615 test */
	#	outb	%al, $0x80

1:
		inb	%dx, %al
#		nop
#		jmp	c0
#c0:
		testb	$0x04, %al
		jz	1b			#wait till idle

		movb	$0x03, %dl
		movb	%bh, %al
		outb	%al, %dx		#spd address
#		nop
#		jmp	c1
#c1:
		movb	$0x01, %dl
		movb	$0x20, %al
		outb	%al, %dx		#set read/write byte command
#		nop
#		jmp	c2
#c2:
		movb	$0x07, %dl
		movb	%bl, %al
		outb	%al, %dx		#set the offset of register
#		nop
#		jmp	c3
#c3:
		movb	$0x02, %dl
		movb	$0x0ff, %al
		outb	%al, %dx		#start command
#		nop
#		jmp	c4
#c4:
/*;wait for command complete*/
		movw	$0x100, %cx		#loop count
		movb	$0x00, %dl	
chk_status_final:
		inb	%dx, %al
#		nop
#		jmp	c5
#c5:
		testb	$0x10, %al
		jnz	send_ok
		loop	chk_status_final
/*;read failed*/
#cheng0606		stc
		jmp	spd_end
send_ok:
		movb	$0x04, %dl
		inb	%dx, %al
#		nop
#		jmp	c6
#c6:
#cheng0606		clc
spd_end:
		RET_SP
		
/*cheng0605 end-----------------------------------------------*/

	.org 0x01f0

reset_vector:
	.byte	0xea				# jmp to fe00:0000, where IPL
	.word	0x0000, DOC_WIN_SEG		# starts in DoC
#.code32
spl_vector:
#	movb	$0x55, %al
#	outb	%al, $0x80		/* cheng 0617 test */

	.byte 	0xea
	.word	0x0000, 0x8000

#	invd
#	jmpl	$0x10, $0x80000
# jmpl	$0x80000
# put this here to buy some room!
pad: 	.byte 0,0,0,0,0,0
