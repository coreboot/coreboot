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
 * modified from ipl.S and converted to 32-bit by S. Gehlbach < steve @ kesa . com>
 * for flash only; docmem not included.
 * $Id$
 */

#include "chipinit.h"

#define SIZE_ALL
#define REALLY_COMPACT

sis630spd_start:
	cli					# Disables the maskable
						# hardware interrupts.
CONSOLE_DEBUG_TX_STRING($str_begin_spd)

	movw	$0x408a, %ax			# ACPI Enable.
	CALL_SP(write_lpc_register)		# (for use of SMBus)

	movw	$0x7550, %ax			# Store ACPI Base Address.
	CALL_SP(write_lpc_register)		# (for use of SMBus)

	movw	$0x5501, %ax			# MDOE# enable, this bit
	CALL_SP(write_pci_register)		# should be set before sizing.

/* -----------------------------------------------------------------------------------------*/

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

	xorl %ecx, %ecx
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

	movl	$sdram_type_bank_2, %esi
	jmp	check_row_column
one_bank:
	movl	$sdram_type_bank_1, %esi

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

	/* This one is DANGEROUS TOO, be careful about OVERFLOW !!! */
	shlb	$0x02, %ch			# row * 4

	addb	%ch, %cl			# column + row *4
	xorb	%ch, %ch

	addl	%ecx, %esi			# sdram_type_bank[column + row * 4]
	movb	-52(%esi), %cl			# sdram_type_bank[column + row * 4 - 52]

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
	orb	$SMA_SIZE, %al			# should be SMA 8 MB for VGA
#endif /* HAVE_FRAMEBUFFER */

#else /* !SIZE_ALL */

no_sdram:
#ifdef HAVE_FRAMEBUFFER
	movb	$0x63, %ah			# enable DIMM 0 and
	movb	$(SMA_SIZE + 0x01), %al		# enable SMA 8 MB for VGA
#else /* HAVE_FRAMEBUFFER */
	movw	$0x6301, %ax			# enable DIMM 0 
#endif /* HAVE_FRAMEBUFFER */

#endif /* SIZE_ALL */

	CALL_SP(write_pci_register)		# write register 0x63

/* -----------------------------------------------------------------------------------------*/

	movl	$pci_init_table, %esi
init_sdram:
	lodsw	(%esi), %ax
	testw	%ax, %ax
	jz	init_complete

	CALL_SP(write_pci_register)
	jmp	init_sdram

init_complete:

////////////////////////////////////////////////////////////////
//
//  We are going to move and reload the gdt, since we are 
//  executing from high mem.  The current gdt is located 
//  above 1M and linux will hang unless the gdt is located <1M.   
//  So we move  the gdt to ram in <1M area. Just under 0x90000
//  is (apparently) a safe spot.
//
////////////////////////////////////////////////////////////////
	CONSOLE_DEBUG_TX_STRING($str_gdt)
#define NEW_GDT_PTR_ADDR   0x0008ff00 // gotta put it somewhere low ram

	movl    $new_gdt_ptr,%esi // source 
	movl    $NEW_GDT_PTR_ADDR,%edi  // find some ram
	movl    $(new_gdt_end-new_gdt_ptr), %ecx   // get length
	shrl    $2, %ecx        // divide by 4 and add 1
	incl    %ecx
	rep
	movsl

// reset the gdt addr to new spot
	movl	$(NEW_GDT_PTR_ADDR+6), (NEW_GDT_PTR_ADDR+2)

.align 4
// now load the new gdt
	lgdt    %cs:NEW_GDT_PTR_ADDR
	ljmp    $0x10, $new_gdt_loaded

new_gdt_loaded:

	CONSOLE_DEBUG_TX_STRING($str_end_spd)
	jmp chipinit0

/* -----------------------------------------------------------------------------------------*/

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
	movw	%cx, %ax			# Restore %ax
	outb	%al, %dx
	RET_SP					# End of write_[lpc|pci]_reg

/* -----------------------------------------------------------------------------------------*/

read_spd:
	/* Input:  AH = 05h, AL = byte number of SPD to be read.
	   Output: BL = The value of specified SPD byte. */
	movb	$0x05, %ah			# set SMB Command == byte address
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
	   Output: BL - register value. */
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

/* -----------------------------------------------------------------------------------------*/

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

/* -----------------------------------------------------------------------------------------*/

pci_init_table:
#	High Byte -> Register	Low Byte -> Value

#include "mainboard/pcchips/m787cl+/dll.inc"

	.word	0x5780	# Precharge
	.word	0x5740	# Mode Register Set
	.word	0x5720	# Refresh
	.word	0x5720	# Refresh
	.word	0x5720	# Refresh
	.word	0x5201	# Refresh Cycle Enable
	.word	0x0000	/* Null, End of table */

/* -----------------------------------------------------------------------------------------*/

sis950_init_table:
	.byte	0x87, 0x01, 0x55, 0x55, 0x24

str_begin_spd: 
	.string "Begin spd and ram sizing routines.\r\n"
str_end_spd:
	.string "  -End spd and ram sizing routines. gdt relocated.\r\n"
str_gdt:
	.string "Relocating gdt...\r\n"

new_gdt_ptr:
.word   0x8000		// gdt limit=2048,
					//  256 GDT entries
.word   0, 0		// gdt base (filled in later)

new_gdt:
.word   0, 0, 0, 0	// dummy
.word   0, 0, 0, 0	// unused

.word   0xFFFF		// 4Gb - (0x100000*0x1000 = 4Gb)
.word   0			// base address = 0
.word   0x9A00		// code read/exec
.word   0x00CF		// granularity = 4096, 386
					//  (+5th nibble of limit)
.word   0xFFFF		// 4Gb - (0x100000*0x1000 = 4Gb)
.word   0			// base address = 0
.word   0x9200		// data read/write
.word   0x00CF		// granularity = 4096, 386
					//  (+5th nibble of limit)
new_gdt_end:

	hlt

chipinit0:
