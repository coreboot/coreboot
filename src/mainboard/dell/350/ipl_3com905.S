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

#include "ipl.h"

.code16
.org 0xfe00

dell350_3com905_start:
	cli					# Disables the maskable
						# hardware interrupts.

	movw	%cs, %ax			# makes data segment ==
	movw	%ax, %ds			#       code segment

	# move the eip to eax, then mask off the bits, 
	# so we have an address for the docipl. This address is used
	# to diddle the docipl so we can read blocks.

	# enable writes on the docipl. 

init_complete:
sis630ipl_start:
	/* O.K. we have DRAM now, so set up STACK for CALL/RET */
	movw	$DOC_STACK_SEG, %ax
	movw	%ax, %ss
	movw	$SPL_RAM_SEG, %ax
	movw	%ax, %es
#ifdef STD_FLASH
	movw	$0xf000, %ax
	movw	%ax, %ds 
	xorw	%sp, %sp			# clear %sp
	xorw	%si, %si
	xorw	%di, %di
	movw	$0xffff, %cx
	rep
	movsb

	movw	$DOC_WIN_SEG, %ax
	movw	%ax, %ds

#else
	movw	$sis950_init_table, %si		# unlock SiS 950 LPC
	movw	$0x05, %cx			# select Clock Selection
	movw	$0x2e, %dx			# and Flash ROM I/F 
	rep
	outsb
	movb	$0xfd, %al			# enable write, CLKIN = 24 MHZ
	outb	%al, $0x2f

	xorw	%sp, %sp			# clear %sp
	xorw	%dx, %dx			# clear %dx, start of RAM
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
0:
	movb	0x20(%di), %al			# read DOC NOP retisger
	loop	0b				# four times

	testb	$0x80, 0x04(%di)		# is flash ready ?
	jz	wait_for_flash_ready

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
#endif /*else STD_FLASH */

sis630ipl_end:
	jmp	spl_vector			# jump to SPL vector

/* ------------------------------------------------------------------------------------------*/

write_lpc_register:
	/* Input: AH - register number. AL - register value. */
	movl	$LPC_BRIDGE_BASE_ADDR, %edx
	jmp	write_common

write_pci_register:
	/* Input: AH - register number. AL - register value. */
	movl	$NORTH_BRIDGE_BASE_ADDR, %edx

write_common:
	movw	%ax, %cx			# Save %ax to %cx.

	movzbl	%ch, %eax			# add register address to
	addl	%edx, %eax			# PCI base address

	movw	$PCI_COMMAND_PORT, %dx
	outl	%eax, %dx

	movw	$PCI_DATA_PORT, %dx
	andb	$0x03, %al
	addb	%al, %dl
	movb	%cl, %al
	outb	%al, %dx
	RET_SP					# End of write_[lpc|pci]_reg

read_spd:
	/* Input:  AH = 05h, AL = byte number of SPD to be read.
	   Output: BL = The value of specified SPD byte. */
	movb	$0x05, %ah
	CALL_BP(sis_set_smbus)

	movw	$0x0312, %ax			# Start, R/W byte Data
	CALL_BP(sis_set_smbus)

wait_for_smbus_read:
	movb	$0x80, %dl			# Read SMBus status 
	inb	%dx, %al

	testb	$0x02, %al			# if device errors
	jnz	read_spd_fail			# then skip read SPD

	testb	$0x08, %al			# if not complete
	jz	wait_for_smbus_read		# then wait SPD data complete

read_spd_fail:
	movb	$0x08, %ah

doc_reset:
	/* Input: AL = value write to DOC_CONTROL register
	   Clobberd: CX */
	movb	%al, 0x02(%di)			# write DoC Control retister
	movb	%al, 0x02(%di)			# twice
	ret					# End of doc_reset

doc_cycle_end:
	movb	$0x00, 0x1e(%di)		# flush write pepeline
	movb	$0x01, 0x04(%di)		# end command cycle
	ret

reset_vector:
#	.byte	0xea				# jmp to fe00:0000, where IPL
#	.word	0x0000, DOC_WIN_SEG		# starts in DoC

	.byte	0xea				# jmp to fe00:0000, where IPL
	.word	0xfe00, 0xf000		# starts in DoC

spl_vector:
	.byte	0xea				# jmp to 8000:0000, where SPL
	.word	0x0000, SPL_RAM_SEG		# (LinuxBIOS) starts in RAM

end:
	hlt
