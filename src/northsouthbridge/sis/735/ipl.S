/*
 * ipl.S:	 Initial Program Loader (IPL) for SiS 735
 *
 * Copyright 2002 Silicon Integrated Systems Corporation
 * Copyright 2002 International Business Machines	
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
 *	1. SiS 735 Programming Guide, Rev. 1.0, Apr. 16, 2001
 *	2. System Management Bus Specification Rev 1.1
 *	3. IBM Application Note, DDR SDRAM Module Serial Presence Detect Definitions
 *	4. JEDEC Standard, DDR SDRAM Speccification, JESD79, June 2000
 *
 * $Id$
 */

#include "ipl.h"

.code16

//#define SPD_SIZING
#define SIZE_ALL
#define REALLY_COMPACT

#ifdef STD_FLASH
.org 0xfc00
#endif

sis735spd_start:
	cli					# Disables the maskable
						# hardware interrupts.

	movw	%cs, %ax			# makes data segment ==
	movw	%ax, %ds			#       code segment

	movw	$pre_dram_sizing_table, %si
pre_dram_sizing_start:
	lodsw	(%si), %ax
	testw	%ax, %ax
	jz	pre_dram_sizing_complete

	CALL_SP(write_north_register)
	jmp	pre_dram_sizing_start

pre_dram_sizing_complete:

	/* According to JEDEC Spec, we have to drive CKE low for at least 200 mu sec
	 * to stabilize DDR. Instead of using some kind of busy delay loop, we
	 * insert some other init stuff before cke_give_up_low and cke_output_normal */
cke_enable_low:
	movw	$0x6B30, %ax			# Bit 5, Enable CKE output
	CALL_BP(write_north_register_or)	# Bit 4, CKE Force Low

acpi_enable:
	movw	$0x408A, %ax			# ACPI Enable.
	CALL_SP(write_lpc_register)		# (for accessing CKE control)

	movw	$0x7550, %ax			# Store ACPI Base Address.
	CALL_SP(write_lpc_register)		# (for accessing CKE control)

	movw	$smbus_init_table, %si
smbus_init_start:				# inits SMBus Host Controller
	lodsw	(%si), %ax			# for accessing SMBus
	testw	%ax, %ax
	jz	smbus_init_complete

	CALL_SP(write_smbus_register)
	jmp	smbus_init_start

smbus_init_complete:

cke_give_up_low:
	movw	$0x5056, %dx			# ACPI 0x56 - legacy event control
	inb	%dx, %al			# Bit 6, Disable Auto-Reset
	orb	$0x60, %al			# Bit 5, CKE control bit for DDR SDRAM
	outb	%al, %dx

cke_output_normal:
	mov	$0x6B, %ah
	CALL_SP(read_north_register)
	and	$0xEF, %al			# Bit 4, CKE Normal
	CALL_SP(write_north_register)

/* -----------------------------------------------------------------------------------------*/
#ifndef SPD_SIZING
ddr_size:
	movw	$0x614A, %ax
	CALL_SP(write_north_register)

	movw	$0x6402, %ax
	CALL_SP(write_north_register)
#else

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

	cmpb	$0x07, %bl			# If the RAM Type == DDR SDRAM ??
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

	/* This one is DANGEROUS TOO, be careful about OVERFLOW !!! */
	shlb	$0x02, %ch			# row * 4

	addb	%ch, %cl			# column + row *4
	xorb	%ch, %ch

	addw	%cx, %si			# sdram_type_bank[column + row * 4]
	movb	-52(%si), %cl			# sdram_type_bank[column + row * 4 - 52]

#ifdef SIZE_ALL
	movb	%ah, %bh			# Restore DIMM slot number from AH to BH
#endif /* SIZE_ALL */

#ifdef SAFTY_CHECK
	cmpb	$0xff, %cl			# SDRAM type supported ??
	je	no_sdram			# no, exit
#endif /* SAFTY_CHECK */

	orb	$0x40, %cl			# set DDR SDRAM bit
						# (reg 0x60~0x63 bit 6)

	movb	$0x05, %al			# Read the Side number (SPD byte 5)
	CALL_SP(read_spd)			# of the dram on current DIMM.

	cmpb	$0x02, %bl			# single or double sided ??
	jne	single_side
	orb	$0x20, %cl			# set double side bit
						# (reg 0x60~0x63 bit 5)

single_side:
	movb	%cl, %al			# store DRAM type in al
	movb	$0x60, %ah			# select register 0x60

#ifdef SIZE_ALL
	addb	%bh, %ah			# select register 0x60~0x63, accroding
						# to DIMM slot number (in BH)
#endif /* SIZE_ALL */
	CALL_SP(write_north_register)		# write register 0x60~0x63 for each
						# DIMM slot

#ifdef SIZE_ALL
	movw	%fs, %ax			# enable DIMMx on reg. 0x64 and save
						# it in FS
	movb	$0x01, %bl
	movb	%bh, %cl
	shlb	%cl, %bl
	orb	%bl, %al
	movw	%ax, %fs
no_sdram:
	incb	%bh
	cmpb	$0x04, %bh			# total 4 DIMM slots supported
	jb	spd_sizing_start

	movw	%fs, %ax
	movb	$0x64, %ah
#else /* !SIZE_ALL */
no_sdram:
	movw	$0x6401, %ax			# enable DIMM 0 
#endif /* SIZE_ALL */

	CALL_SP(write_north_register)		# write DRAM status register 0x64

#endif

/* -----------------------------------------------------------------------------------------*/

	/* Start JEDEC DDR initialization sequence */
jedec_ddr_init_seq:
	movb	NO_OP, %bl
	CALL_BP(dram_ctrl_cmd)

	movw	$ddr_init_table, %si
init_ddr_sdram:
	lodsb	(%si), %al
	testb	%al, %al
	jz	ddr_init_complete

	movb	%al, %bl
	CALL_BP(dram_ctrl_cmd)
	jmp	init_ddr_sdram

ddr_init_complete:

	movw	$post_dram_init_table, %si
post_dram_init_start:
	lodsw	(%si), %ax
	testw	%ax, %ax
	jz	post_dram_init_complete

	CALL_SP(write_north_register)
	jmp	post_dram_init_start

post_dram_init_complete:

/* -----------------------------------------------------------------------------------------*/

sis735ipl_start:
	/* O.K. we have DRAM now, so set up STACK for CALL/RET */
	movw	$DOC_STACK_SEG, %ax
	movw	%ax, %ss
	movw	$SPL_RAM_SEG, %ax
	movw	%ax, %es
	xorw	%sp, %sp			# clear %sp

#ifdef STD_FLASH
#include "rom/std_flash.inc"
#else /* !STD_FLASH */
#include "unlock_flash.inc"
#if defined(USE_DOC_MIL) || defined(USE_DOC_2000_TSOP)
#	include "rom/doc_mil.inc"
#else defined(USE_DOC_MIL_PLUS)
#	include "rom/doc_mil_plus.inc"
#endif
#endif /* STD_FLASH */

sis735ipl_end:
	jmp	spl_vector			# jump to SPL vector


/* -----------------------------------------------------------------------------------------*/

write_north_register:
	/* Input: AH - register number. AL - register value.
	   Chnaged: CX, EDX */
	movl	$NORTH_BRIDGE_BASE_ADDR, %edx
	jmp	write_common

write_lpc_register:
	/* Input: AH - register number. AL - register value.
	   Chnaged: CX, EDX */
	movl	$LPC_BRIDGE_BASE_ADDR, %edx
	jmp	write_common

write_smbus_register:
	/* Input: AH - register number. AL - register value.
	   Chnaged: CX, EDX */
	movl	$SMBUS_HOST_BASE_ADDR, %edx
	jmp	write_common

write_common:
	movw	%ax, %cx			# Save %ax to %cx.

	movzbl	%ch, %eax			# add register address to
	addl	%edx, %eax			# PCI base address

	movw	$PCI_COMMAND_PORT, %dx
	outl	%eax, %dx

	movw	$PCI_DATA_PORT, %dx
	andb	$0x03, %al
	addb	%al, %dl
	movw	%cx, %ax
	outb	%al, %dx
	RET_SP					# End of write_[pci|smbus]_reg

/* -----------------------------------------------------------------------------------------*/
read_north_register:
	/* Input: AH - register number.
	   Output AL - register value.
	   Chnaged: CX, EDX */
	movl	$NORTH_BRIDGE_BASE_ADDR, %edx

read_common:
	movw	%ax, %cx			# Save %ax to %cx.

	movzbl	%ch, %eax			# add register address to
	addl	%edx, %eax			# PCI base address

	movw	$PCI_COMMAND_PORT, %dx
	outl	%eax, %dx

	movw	$PCI_DATA_PORT, %dx
	andb	$0x03, %al
	addb	%al, %dl
	inb	%dx, %al

	movb	%ch, %ah			# Resotre register number to %ah

	RET_SP					# End of write_[pci|smb]

write_north_register_or:
	/* Input: AH - register number. AL - register mask.
	   Chnaged: CX, EDX*/
	CALL_SP(read_north_register)		# %cl == old %al
	orb	%cl, %al
	CALL_SP(write_north_register)
	RET_BP

/* -----------------------------------------------------------------------------------------*/

dram_ctrl_cmd:
	/* Input: BL - DRAM COMMAND.
	   Chnaged: AX, CX, EDX*/
	movb	$0x5C, %ah			# read 0x5C DRAM Init Control
	CALL_SP(read_north_register)
	andb	$0x07, %al			# Clear Command bits
	orb	%bl, %al			# Set DRAM command
	CALL_SP(write_north_register)
	orb	$0x08, %al			# Issue DRAM Command
	CALL_SP(write_north_register)
	RET_BP

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
	.byte	0b0001,	0b0101,	0b1001,	0xff	# 13

sdram_type_bank_2:	
# Column Number	  8	  9	  10	  11	Row Number
	.byte	0b1100,	0xff,	0xff,	0xff	# 11
	.byte	0b0010,	0b0110,	0b1010,	0b1110	# 12
	.byte	0b0011,	0b0111,	0b1011,	0b1111	# 13
# FixMe: Bank 2 x Column 12 x Row 13 == 1GB

/* -----------------------------------------------------------------------------------------*/

pre_dram_sizing_table:
#	High Byte -> Register	Low Byte -> Value
	/* DRAM Control */
	.word	0x5929				# CAS Latency = 2.5T
	.word	0x5c12				# MD/DQS Input Enable for DDR
	.word	0x5dc0				# Refresh Cycle Disabled

	/* ACPI Control */
	.word	0x6801				# ACPI Enable.
	.word	0x6950				# Store ACPI Base Address.

	/* IO Buffer */
	.word	0x9910				# DDR IO Input Mode ==> Differential
	.word	0x9ab8				# Enable DQS signal delay

	.word	0x0000	/* Null, End of table */

post_dram_init_table:
#	High Byte -> Register	Low Byte -> Value
	.word	0x0407				# Enable IO/MEM Space, BM DMA
	.word	0x0d20

	/* DRAM Control */
	.word	0x5dc1				# Refresh Cycle Enabled

	.word	0x0000	/* Null, End of table */

smbus_init_table:
#	High Byte -> Register	Low Byte -> Value
	.word	0x0401				# enable IO space
	.word	0x2080				# SMBus Base address =
	.word	0x2150				# 0x5080
	.word	0x4001				# enable SMBus Host Controller
	.word	0x0000	/* Null, End of table */

ddr_init_table:
	.byte	PALL				# Percharge All
	.byte	DLL_EN_N			# DLL Enable Normal
	.byte	DLL_RESET			# DLL Reset
	.byte	PALL				# Precharge All
	.byte	REF				# Refresh
	.byte	REF				# Refresh
	.byte	MRS				# Mode Register Set
	.byte	0x0000	/* Null, End of table */

/* -----------------------------------------------------------------------------------------*/
#ifdef STD_FLASH
	.org 0xfff0
reset_vector:
	.byte	0xea				# jmp to f000:fc00, where IPL
	.word	0xfc00, 0xf000			# starts in Standard Flash
#else /* !STD_FLASH i.e. DoC Mil */
#if defined(USE_DOC_MIL)
	.org 0x1f0
#elif defined(USE_DOC_2000_TSOP) || defined(USE_DOC_MIL_PLUS)
	.org 0x3f0
#endif
reset_vector:
	.byte	0xea				# jmp to fc00:0000, where IPL
	.word	0x0000, DOC_WIN_SEG		# starts in DoC
#endif /* STD_FLASH */
	
spl_vector:
	.byte	0xea				# jmp to 8000:0000, where SPL
	.word	0x0000, SPL_RAM_SEG		# (LinuxBIOS) starts in RAM

/* -----------------------------------------------------------------------------------------*/

sis950_init_table:
	.byte	0x87, 0x01, 0x55, 0x55, 0x24

end:
	hlt
