/*
 * ipl.S:	 Initial Program Loader (IPL) for SiS 550 SoC and M-System DoC Millennium
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
 *	1. SiS 550 Chipset Registers Version 0.1, Mar. 8, 2001
 *	2. System Management Bus Specification Rev 1.1
 *	3. PC SDRAM Serial Presence Detect (SPD) Specification Rev 1.2B
 *	4. Booting From the DiskOnChip Millennium, M-Systems Application Note Ap-DOC-044
 *	   APR-2000, 93-SR-001-44-7L REV. 1.0
 *
 * $Id$
 */

#include "ipl.h"

.code16

#ifdef STD_FLASH
.org 0xfc00
#endif

#undef SIZE_ALL
#define REALLY_COMPACT

sis630spd_start:
	cli					# Disables the maskable
        movw $0,%cx
reset:						# hardware interrupts.

	movw	%cs, %ax			# makes data segment ==
	movw	%ax, %ds			#       code segment

	movw	$lpc_init_table, %si
lpc_init_start:					# inits LPC bridge for accessing
	lodsw	(%si), %ax			# SMBus and DoC
	testw	%ax, %ax
	jz	lpc_init_complete

	CALL_SP(write_lpc_register)
	jmp	lpc_init_start
        cmp     $1,%cx
        je lpc_init_complete

lpc_init_complete: 

Check_CMOS:
       movw $0x0070,%dx
       movb $0x53,%al                  
       outb %al,%dx
       movw $0x0071,%dx
       inb  %dx,%al
       cmpb $0x5a,%al
       jne  Mininum_Setting
       
       movw $0x0070,%dx
       movb $0x54,%al
       outb %al,%dx
       movw $0x0071,%dx
       inb  %dx,%al
       cmpb $0xa5,%al
       je   SET_VGA_SR13

Mininum_Setting:
       //Setting the flag 
       movw $0x0070,%dx
       movb $0x53,%al
       outb %al,%dx
       movb $0x5a,%al 
       movw $0x0071,%dx
       outb %al,%dx
      
       movw $0x0070,%dx
       movb $0x54,%al
       outb %al,%dx
       movb $0xa5,%al
       movw $0x0071,%dx
       outb %al,%dx
       
       //Set the Host CPU/SDRAM/PCI to the minimum frequency
       movw $0x0070,%dx
       movb $0x50,%al
       outb %al,%dx
       movb $0x00,%al
       movw $0x0071,%dx
       outb %al,%dx
  
       //Set the SR13 to the minimun value
       movw $0x0070,%dx
       movb $0x51,%al
       outb %al,%dx
       movb $0x00,%al
       movw $0x0071,%dx
       outb %al,%dx

       //Set the Shar Memory Size to 8M
       movw $0x0070,%dx
       movb $0x70,%al
       outb %al,%dx
       movb $0x20,%al
       movw $0x0071,%dx
       outb %al,%dx
       
       //Set SR14 to 0x03   
       movw $0x0070,%dx
       movb $0x71,%al
       outb %al,%dx
       movb $0x03,%al
       movw $0x0071,%dx
       outb %al,%dx

       //Set Boot disk as master disk       
       movw $0x0070,%dx
       movb $0x62,%al
       outb %al,%dx
       movb $0x01,%al
       movw $0x0071,%dx
       outb %al,%dx

       //Set SR16 to 0x00 
       movw $0x0070,%dx
       movb $0x61,%al
       outb %al,%dx
       movb $0x00,%al
       movw $0x0071,%dx
       outb %al,%dx
       //Disable the flash write function
       movw $0x0070,%dx
       movb $0x52,%al
       outb %al,%dx
       movb $0x00,%al
       movw $0x0071,%dx
       outb %al,%dx


SET_VGA_SR13:
       movw $0x03c4,%dx           #SR5
       movb $0x05,%al
       outb %al,%dx
       movw $0x03c5,%dx           #Enable
       movb $0x86,%al
       outb %al,%dx

       movw $0x0070,%dx           
       movb $0x51,%al             #SR13
       outb %al,%dx
       movw $0x0071,%dx
       inb  %dx,%al
       movb %al,%bl
       andb $0xc0,%al
       cmpb $0x80,%al
       jne SET_VGA_SR14
              
       movw $0x03c4,%dx
       movb $0x13,%al
       outb %al,%dx
       movw $0x03c5,%dx
       movb %bl,%al
       andb $0x3f,%al
       outb %al,%dx

SET_VGA_SR14: 
       movw $0x0070,%dx
       movb $0x71,%al             #SR14
       outb %al,%dx
       movw $0x0071,%dx
       inb  %dx,%al
       movb %al,%bl
       andb $0x3f,%al
       cmpb $0x0f,%al
       jg   SET_VGA_SR16           

       movw $0x03c4,%dx
       movb $0x14,%al
       outb %al,%dx
       movw $0x03c5,%dx
       movb %bl,%al
       outb %al,%dx

SET_VGA_SR16:
       movw $0x0070,%dx
       movb $0x61,%al             #SR16
       outb %al,%dx
       movw $0x0071,%dx
       inb  %dx,%al
       movb %al,%bl

       movw $0x03c4,%dx
       movb $0x16,%al
       outb %al,%dx
       movw $0x03c5,%dx
       movb %bl,%al
       outb %al,%dx

SET_VGA_COMPLETE:




        movw $0x0080,%dx
        movb $1,%al
        outb %al,%dx

        //Get Base Address
        movw $0xcf8,%dx
        movl $0x80000874,%eax
        outl %eax,%dx
        movw $0xcfc,%dx
        inl  %dx,%eax
        movw %ax,%dx

        //Disable Watch Dog Timer
        addw $0x4b,%dx              #set 0x4b=0x00
        movb $0x00,%al
        outb %al,%dx
      
 
        movw $0x0080,%dx
        movb $2,%al
        outb %al,%dx

        //Increse The Retry Counter
        movw $0x0070,%dx
        movb $0x60,%al
        outb %al,%dx
        movw $0x0071,%dx
        inb  %dx,%al
        addb $1,%al
        outb %al,%dx


Clock_Gen:
        //Get Complete Flag & Parameter
        movw $0x0070,%dx
        movb $0x50,%al
        outb %al,%dx
        movw $0x0071,%dx
        inb  %dx,%al
        movb %al,%bl

        andb $0x01,%al
        cmpb $1,%al
        jne  Clock_Gen_Backup
        movb %bl,%al 
        andb $0x08,%al
        cmpb $0x08,%al
        jne  Clock_Gen_Backup   
        movb $0,%al    
        movb %al,%bl        
 
Clock_Gen_Backup:
        movw $0x0080,%dx
        movb $3,%al
        outb %al,%dx

        //backup the parameter
        movb %bl,%al
        andb $0x02,%al
        cmpb $0x02,%al
        je   Clock_Gen_Exit



Clock_Gen_Adjust:
        //Get The Retry Counter
        movw $0x0070,%dx
        movb $0x60,%al
        outb %al,%dx
        movw $0x0071,%dx
        inb  %dx,%al
        cmpb $3,%al               //It can try 2 times
        jl   Clock_Gen_Start

        //Clear The Retry Counter
        movw $0x0070,%dx
        movb $0x60,%al
        outb %al,%dx
        movb $0,%al
        movw $0x0071,%dx
        outb %al,%dx


        movb $0,%al                     #degrade to default   
        //Backup The New Parameter
        movb %al,%bl

             

Clock_Gen_Start:
        movw $0x0080,%dx
        movb $4,%al
        outb %al,%dx

        //Restor The Parameter
        movw $0x0070,%dx
        movb $0x50,%al
        outb %al,%dx
        movb %bl,%al
        movw $0x0071,%dx
        outb %al,%dx



        //Get Base Address
        movw $0xcf8,%dx
        movl $0x80000874,%eax
        outl %eax,%dx
        movw $0xcfc,%dx
        inl  %dx,%eax
        movw %ax,%dx

        //Set Clock
        addw $0x83,%dx           #set 0x83=0x20 KILL Operation
        movb $0x20,%al
        outb %al,%dx
        addw $1,%dx              #set 0x84=0xd2,Salve=0xd2 Write operation
        movb $0xd2,%al
        outb %al,%dx
        addw $3,%dx              #set 0x87=1 , ByteCount=1
        movb $1,%al
        outb %al,%dx
        addw $1,%dx              #set 0x88=clock
        movb %bl,%al
        andb $0xf4,%al   
        orb  $0x08,%al           #Clock Set by Software
        outb %al,%dx
        subw $5,%dx              #set 0x83=0x05, Block write
        movb $5,%al
        outb %al,%dx
        movb $0x15,%al           #set 0x83=0x15, Start Block Write
        outb %al,%dx

        movw $0x0080,%dx
        movb $5,%al
        outb %al,%dx

        //Get Base Address
        movw $0xcf8,%dx
        movl $0x80000874,%eax
        outl %eax,%dx
        movw $0xcfc,%dx
        inl  %dx,%eax
        movw %ax,%dx
       
        addw $0x80,%dx
        movw $0xffff,%cx 

Clock_Gen_Wait:
        inb %dx,%al
        andb $0x08,%al
        cmpb $0x08,%al
        je Clock_Gen_Complete
        loop Clock_Gen_Wait 

Clock_Gen_Fail:

        movw $0x0080,%dx
        movb $6,%al
        outb %al,%dx

        //Get Base Address
        movw $0xcf8,%dx
        movl $0x80000874,%eax
        outl %eax,%dx
        movw $0xcfc,%dx
        inl  %dx,%eax
        movw %ax,%dx


        //Set Watch Dog Timer(200 ms)
        addw $0x4a,%dx           #set 0x4a=50
        movb $50,%al
        outb %al,%dx
        addw $1,%dx              #set 0x4b=0x83
        movb $0x83,%al
        outb %al,%dx

        movw $0x0080,%dx
        movb $7,%al
        outb %al,%dx


Clock_Gen_Delay1:
        NOP 
        jmp Clock_Gen_Delay1

Clock_Gen_Complete:
       //Set the Complete Flag
        movw $0x0070,%dx
        movb $0x50,%al
        outb %al,%dx
        movw $0x0071,%dx
        inb  %dx,%al
        orb  $0x02,%al
        outb %al,%dx

        movw $0x0080,%dx
        movb $8,%al
        outb %al,%dx


        //Get Base Address
        movw $0xcf8,%dx
        movl $0x80000874,%eax
        outl %eax,%dx
        movw $0xcfc,%dx
        inl  %dx,%eax
        movw %ax,%dx

        //Set Watch Dog Timer(200 ms)
        addw $0x4a,%dx           #set 0x4a=50
        movb $50,%al
        outb %al,%dx
        addw $1,%dx              #set 0x4b=0x83
        movb $0x83,%al
        outb %al,%dx


        movw $0x0080,%dx
        movb $9,%al
        outb %al,%dx

Clock_Gen_Delay2:
        NOP
        jmp Clock_Gen_Delay2


Clock_Gen_Exit:
        //Clear the Complete Flag
        movw $0x0070,%dx
        movb $0x50,%al
        outb %al,%dx
        movw $0x0071,%dx
        inb  %dx,%al
        andb $0xfd,%al
        outb %al,%dx

        //Clear The Retry Counter
        movw $0x0070,%dx
        movb $0x60,%al
        outb %al,%dx
        movb $0,%al
        movw $0x0071,%dx
        outb %al,%dx
        
        movw $0x0080,%dx
        movb $0x10,%al
        outb %al,%dx


	movw	$0x5501, %ax			# Mode# enable, this bit
	CALL_SP(write_pci_register)		# should be set before sizing.

#ifdef SIZE_ALL
	xorw	%bx, %bx			# clear %fs, %fs is used as "bitmap" of
	movw	%bx, %fs			# installed DIMM slot
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

//	orb	$SMA_SIZE, %al			# enable SMA 8 MB for VGA
#endif /* HAVE_FRAMEBUFFER */

#else /* !SIZE_ALL */

no_sdram:
#ifdef HAVE_FRAMEBUFFER
	movb	$0x63, %ah			# enable DIMM 0 and

        movw    $0x0070,%dx
        movb    $0x70,%al
        outb    %al,%dx
        movw    $0x0071,%dx
        inb     %dx,%al
        andb    $0x70,%al
        cmpb    $0x70,%al                       #Not supported yet
        jne     CHECK_NEXT
        movb    $0x20,%al                       #8MB
        jmp     CHECK_OK
CHECK_NEXT:
        cmpb    $0x60,%al                       #Not supported yet
        jne     CHECK_OK
        movb    $0x20,%al                       #8MB   
CHECK_OK:
        orb     $0x81,%al

//	movb	$(0xB0 + 0x01), %al		# enable SMA 8 MB for VGA
#else /* HAVE_FRAMEBUFFER */
	movw	$0x6301, %ax			# enable DIMM 0 
#endif /* HAVE_FRAMEBUFFER */

#endif /* SIZE_ALL */

	CALL_SP(write_pci_register)		# write register 0x63

#if 0	
	movw	$0x6006, %ax
	CALL_SP(write_pci_register)		# write register 0x60~0x62 for each
						# DIMM slot
	movw	$0x63a1, %ax			# enable DIMM 0 
	CALL_SP(write_pci_register)		# write register 0x63	
#endif

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
	xorw	%sp, %sp			# clear %sp

#ifdef STD_FLASH
#include "rom/std_flash.inc"
#else /* !STD_FLASH */
#if (USE_DOC_MIL == 1) || (USE_DOC_2000_TSOP == 1)
#	include "rom/doc_mil.inc"
#else (USE_DOC_MIL_PLUS == 1)
#	include "rom/doc_mil_plus.inc"
#endif
#endif /* STD_FLASH */

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

lpc_init_table:
#	High Byte -> Register	Low Byte -> Value
	.word	0x408a				# ACPI Enable, Decode E,F segment
	.word	0x4540				# Flash Write Enable. (for DoC command)

	.word	0x78fc
	.word	0x79fc
	.word	0x7afe
	.word	0x7bfe

	.word	0x7550				# Store ACPI Base Address
      
						# (for use of SMBus)
	.word	0x0000	/* Null, End of table */
	
pci_init_table:
#	High Byte -> Register	Low Byte -> Value

//#include "dll.inc"

	.word	0x8c44				# SDRCLK/SDWCLK
	.word	0x8d86				# SDWCLK

	.word	0x5780				# Precharge
	.word	0x5200				# Refresh Cycle Disable
	.word	0x5780				# Precharge
	.word	0x5740				# Mode Register Set
	.word	0x5720				# Refresh
	.word	0x5720				# Refresh
	.word	0x5720				# Refresh
	.word	0x5201				# Refresh Cycle Enable
	.word	0x0000	/* Null, End of table */

#ifdef STD_FLASH
	.org 0xfff0
reset_vector:
	.byte	0xea				# jmp to f000:fc00, where IPL
	.word	0xfc00, 0xf000			# starts in Standard Flash
#else /* !STD_FLASH i.e. DoC Mil */
#if (USE_DOC_MIL == 1)
	.org 0x1f0
#elif (USE_DOC_2000_TSOP == 1) || (USE_DOC_MIL_PLUS == 1)
	.org 0x3f0
#endif
reset_vector:
	.byte	0xea				# jmp to fe00:0000, where IPL
	.word	0x0000, DOC_WIN_SEG		# starts in DoC
#endif /* STD_FLASH */

spl_vector:
	.byte	0xea				# jmp to 8000:0000, where SPL
	.word	0x0000, SPL_RAM_SEG		# (LinuxBIOS) starts in RAM

#ifdef STD_FLASH
	.org 0xffff
#else /* !STD_FLASH i.e. DoC Mil */
#if (USE_DOC_MIL == 1)
	.org 0x1ff
#elif (USE_DOC_2000_TSOP == 1) || (USE_DOC_MIL_PLUS == 1)
	.org 0x3ff
#endif
#endif

end:
	hlt
