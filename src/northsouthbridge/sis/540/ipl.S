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

#define SIZE_ALL
#define REALLY_COMPACT

sis630spd_start:
	cli					# Disables the maskable
						# hardware interrupts.

	movw	%cs, %ax			# makes data segment ==
	movw	%ax, %ds			#       code segment

	movw	$0x408a, %ax			# ACPI Enable.
	CALL_SP(write_lpc_register)		# (for use of SMBus)

	movw	$0x7550, %ax			# Store ACPI Base Address.
	CALL_SP(write_lpc_register)		# (for use of SMBus)

	movw	$0x5501, %ax			# Mode# enable, this bit
	CALL_SP(write_pci_register)		# should be set before sizing.

#ifdef SIZE_ALL
	xorw	%bx, %bx			# clear %fs, %fs is used as "bitmap" of
	movw	%bx, %fs			# install DIMM slot
#endif /* SIZE_ALL */

spd_sizing_start:
#ifndef REALLY_COMPACT
	movw	$0x0320, %ax			# Issue an SMB_Kill command to
	CALL_BP(sis_set_smbus)			# stop all SMBus operation
#endif /* REALLY_COMPACT */

	movw	$0x04a1, %ax			# SPD is on SMBUS Address 1010 xyz1
						# where xyz are DIMM Slot Number
#ifdef SIZE_ALL
	addb	%bh, %al			# FIXME, %bh == 0 ??
	addb	%bh, %al			# Select the DIMM to be SPD-sized.
#endif /* SIZE_ALL*/

	CALL_BP(sis_set_smbus)

	movb	$0x02, %al			# Read the RAM Type (SPD byte 2)
	CALL_SP(read_spd)			# of the dram on current DIMM.

	cmpb	$0x04, %bl			# If the RAM Type = SDRAM ??
	jne	no_sdram			# no, exit

	movb	$0x03, %al			# Read the Row number (SPD byte 3)
	CALL_SP(read_spd)			# of the dram on current DIMM.
	movb	%bl, %ch			# save the Row number in CH.

	movb	$0x04, %al			# Read the Column number (SPD byte 4)
	CALL_SP(read_spd)			# of the dram on current DIMM.
	movb	%bl, %cl			# Save the Column number in CL.

	movb	$0x11, %al			# Read the Bank number (SPD byte 17)
	CALL_SP(read_spd)			# of the dram on current DIMM.

#ifdef SIZE_ALL
	movb	%bh, %ah			# Save the current DIMM slot number in AH
#endif

	cmpb	$0x01, %bl
	je	one_bank

	movw	$sdram_type_bank_2, %si
	jmp	check_row_column
one_bank:
	movw	$sdram_type_bank_1, %si

check_row_column:
#ifdef SAFTY_CHECK
	cmpb	$0x0b, %ch			# Row number too small, unsupported.
	jb	no_sdram
	cmpb	$0x0d, %ch			# Row number too big, unsupported.
	jl	no_sdram
	cmpb	$0x08, %cl			# Col number too small, unsupported.
	jb	no_sdram
	cmpb	$0x0b, %cl			# Col number too big, unsupported.
	jl	no_sdram
#endif /* SAFTY_CHECK */

#if 1
	subb	$0x0b, %ch			# row = row - 11
	shlb	$0x02, %ch			# row * 4

	addb	%ch, %cl			# column + row * 4
	movzbw	%cl, %bx			# Get the SDRAM type.

	addw	%bx, %si			# sdram_type_bank[column + row * 4]
	movb	-8(%si), %cl			# sdram_type_bank[column + row * 4 - 8]
#else
	/* This one is DANGEROUS TOO, be careful about OVERFLOW !!! */
	shlb	$0x02, %ch			# row * 4

	addb	%ch, %cl			# column + row *4
	xorb	%ch, %ch

	addw	%cx, %si			# sdram_type_bank[column + row * 4]
	movb	-52(%si), %cl			# sdram_type_bank[column + row * 4 - 52]
#endif

#ifdef SIZE_ALL
	movb	%ah, %bh			# Restore DIMM slot number from AH to BH
#endif /* SIZE_ALL */

#ifdef SAFTY_CHECK
	cmpb	$0xff, %cl			# SDRAM type supported ??
	je	no_sdram			# no, exit
#endif /* SAFTY_CHECK */

	movb	$0x05, %al			# Read the Side number (SPD byte 5)
	CALL_SP(read_spd)			# of the dram on current DIMM.

	cmpb	$0x02, %bl			# single or double sided ??
	jne	single_side
	orb	$0x20, %cl			# set double side bit
						# (reg 0x60~0x62 bit 5)

single_side:
	movb	%cl, %al			# store DRAM type in al
	movb	$0x60, %ah			# select register 0x60

#ifdef SIZE_ALL
	addb	%bh, %ah			# select register 0x61, 0x62, accroding
						# to DIMM slot number (in BH)
#endif /* SIZE_ALL */
	CALL_SP(write_pci_register)		# write register 0x60~0x62 for each
						# DIMM slot

#ifdef SIZE_ALL
	movw	%fs, %ax			# enable DIMMx on reg. 63h and save
						# it in FS
	movb	$0x01, %bl
	movb	%bh, %cl
	shlb	%cl, %bl
	orb	%bl, %al
	movw	%ax, %fs
no_sdram:
	incb	%bh
	cmpb	$0x03, %bh			# total 3 DIMM slots supported
	jb	spd_sizing_start

	movw	%fs, %ax
	movb	$0x63, %ah
#ifdef HAVE_FRAMEBUFFER
	orb	$SMA_SIZE, %al			# enable SMA 8 MB for VGA
#endif /* HAVE_FRAMEBUFFER */

#else /* !SIZE_ALL */

no_sdram:
#ifdef HAVE_FRAMEBUFFER
	movb	$0x63, %ah			# enable DIMM 0 and
	movw	$SMA_SIZE + $0x01, %ax		# enable SMA 8 MB for VGA
#else /* HAVE_FRAMEBUFFER */
	movw	$0x6301, %ax			# enable DIMM 0 
#endif /* HAVE_FRAMEBUFFER */

#endif /* SIZE_ALL */

	CALL_SP(write_pci_register)		# write register 0x63

	movw	$pci_init_table, %si
init_sdram:
	lodsw	(%si), %ax
	testw	%ax, %ax
	jz	init_complete

	CALL_SP(write_pci_register)
	jmp	init_sdram

init_complete:
sis630ipl_start:
	/* O.K. we have DRAM now, so set up STACK for CALL/RET */
	movw	$DOC_STACK_SEG, %ax
	movw	%ax, %ss
	movw	$SPL_RAM_SEG, %ax
	movw	%ax, %es

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

sis_get_smbus:
	/* Input:  AH - register index.
	   Output: AL - register value. */
	addb	%ah, %dl			# read SMBus byte 0
	inb	%dx, %al
	movb	%al, %bl			# return result in BL

	movw	$0x00ff, %ax
	CALL_BP(sis_set_smbus)			# clear ACPI 80h status

	RET_SP					# End of read_spd

sis_set_smbus:
	/* Input: AH - register index. AL - register value. */
	movw	$SMB_BASE_ADDR, %dx
	addb	%ah, %dl
	outb	%al, %dx
	RET_BP					# End of sis_set_smbus

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

sdram_type_bank_1:
# Column Number	  8	  9	  10	  11	Row Number
	.byte	0b0000,	0b0100,	0b1000,	0xff	# 11
	.byte	0xff,	0xff,	0xff,	0xff	# 12
	.byte	0b0001,	0b0101,	0b1001,	0b1101	# 13

sdram_type_bank_2:	
# Column Number	  8	  9	  10	  11	Row Number
	.byte	0b1100,	0xff,	0xff,	0xff	# 11
	.byte	0b0010,	0b0110,	0b1010,	0b1110	# 12
	.byte	0b0011,	0b0111,	0b1011,	0b1111	# 13

pci_init_table:
#	High Byte -> Register	Low Byte -> Value

//#include "dll.inc"

	.word	0x8c44	# Precharge
	.word	0x8d86	# Mode Register Set
	.word	0x8ec1	# Refresh
	.word	0x8f46	# Refresh

	.word	0x5780	# Precharge
	.word	0x5740	# Mode Register Set
	.word	0x5720	# Refresh
	.word	0x5720	# Refresh
	.word	0x5720	# Refresh
	.word	0x5201	# Refresh Cycle Enable
	.word	0x0000	/* Null, End of table */

	.org 0x01f0
reset_vector:
	.byte	0xea				# jmp to fe00:0000, where IPL
	.word	0x0000, DOC_WIN_SEG		# starts in DoC

spl_vector:
	.byte	0xea				# jmp to 8000:0000, where SPL
	.word	0x0000, SPL_RAM_SEG		# (LinuxBIOS) starts in RAM

sis950_init_table:
	.byte	0x87, 0x01, 0x55, 0x55, 0x24

end:
	hlt
