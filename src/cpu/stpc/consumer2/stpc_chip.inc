/*
 * Bootstrap code for the STPC Consumer II
 *
 * By Steve M. Gehlbach
 *
 * Heavily changed; pretty much only for the ConsumerII.
 *
 * originally by:
 * johryd
 * Copyright (c) 1999 by Net Insight AB. All Rights Reserved.
 *
 *
 */

#include "cpu/stpc/consumer2/stpc.h"
/*
 * Initialize the STPC chip based on the table
 * at the end of this file.
 *
 * stpc_chip_init:
 */
stpc_chip_init:
	/* 
	 * Disable shadow 0xf000, since we already use
	 * the link register (%sp), we can't call the
	 * ``stpc_chip_write´´ routine. 
	 */
	movb	$0x11, %al
	outb	%al,$0x80

	xorb	%al, %al
	movb	$0x28, %ah
	stpc_conf_write_macro

	jmp	.Lflush			/* make sure now fetched from rom
					     * not shadow after soft reset */
	.balign	16
.Lflush:

	//
	// move the table address and start the
	// loop
	//
	movl	$stpc_chip_table, %esi
.Lget:	xorb	%dh, %dh		/* clear %dh */

	/* get the port */
	movb	%cs:(%esi), %dl
	cmpb	$0xff, %dl
	je	.Ldone
	
	/* add 1 to pointer */
	inc	%esi

	/* get data and add 2 to pointer*/
	movb	%cs:(%esi), %ch		/* get mask */
	inc	%esi
	movb	%cs:(%esi), %cl		/* get data */
	inc	%esi
	
	/* send index to index register */
	movw	%dx, %ax
	outb	%al, $STPC_CONF_REG_INDEX
	
	/* read data, mask and set */
	inb	$STPC_CONF_REG_DATA, %al
	andb	%ch, %al
	orb	%cl, %al
	movb	%al, %cl
	
	/* send data to register */
	movw	%dx, %ax
	outb	%al, $STPC_CONF_REG_INDEX
	movb	%cl, %al
	outb	%al, $STPC_CONF_REG_DATA

	/* loop back */
	jmp	.Lget
.Ldone:

        /****************************************
         *                                      *
         *     set chip refresh                 *
         *     set ISA refresh on               *
         *                                      *
         ****************************************/
/*
 * stpc_chip_refresh:
 */
stpc_chip_refresh:
	/*
	 * This has been upgraded to include CONSUMER_II timings
	 * First of all we get the bus speed
	 * into the %al register
	 */

	xorl	%eax, %eax
	stpc_chip_getbusspeed_macro
 	movl	$stpc_dram_refresh, %ebx
 	xorb	%ah, %ah
	addl	%eax, %ebx			// offset into the table 
	movb	%cs:0(%ebx), %al 	// load refresh value

	movb	$0x39, %ah	/* refresh control reg */
	stpc_conf_write_macro

// set isa refresh on

        movb    $0x57, %ah
        stpc_conf_read_macro  //read value into %al
        orb             $0b00000001, %al
        stpc_conf_write_macro

// make sure any coprocessor is properly reset
        xorw    %ax, %ax
        outb    %al, $0xf0
                stpc_delay_macro(10)

        outb    %al, $0xf1
                stpc_delay_macro(10)

	jmp	stpc_chip0
	

/*
 * stpc_chip_table:
 * format is reg addr, mask, data
 * reg is first read into register
 * mask is anded into register, data is ored into register
 *	Data to initialize the chip. 
 */
stpc_chip_table:

 /*
  *	Cache/CPU interface registers:
  */
 	.byte	0xC1, 0b11100000, 0b00000000	// clear no lock
 	.byte	0xC2, 0b00000001, 0b00000010	// enable write back
 											//  enable BARB disable WT rom
 	.byte	0x20, 0b00000000, 0b00101000	// synch burst SRAM
											// support L1 write-back
 	.byte	0x21, 0b00000001, 0b10100000	// cache size 2MB
 	.byte	0x22, 0b10000000, 0b01111111	// slowest timings
 	.byte	0x24, 0b00000000, 0b00000000	/* no ISA mem hole */	
 
/*
 *	Shadow:
 */
	.byte	0x25, 0b00000000, 0b00000000	/* shadow disable for C000 seg */
	.byte	0x26, 0b00000000, 0b00000000	/* shadow disable for D000 seg */
	.byte	0x27, 0b00000000, 0b00000000	/* shadow disable for E000 seg */
	.byte	0x28, 0b00011100, 0b00000000	/* shadow disable for F000 seg , enable VGA for A0000*/
	
/*
 *	ISA interface:
 */
 	.byte	0x59, 0b11111110, 0b00000000	/* ISA synchronizer on */
 	.byte	0x50, 0b00000000, 0b00000000	/* ISA Misc 0: 14MHz/2 no post, enable kbd, a20+reset */
/* 
 *	ROM chip select:			(ISA INTERFACE, page 13/17)
 */
	.byte	0x51, 0b00010000, 0b00001000	// ISA Misc 1: ROM write protect
						 					// segments E,D,C segment share off
/*
 *	IPC:
 */
 	.byte	0x01, 0b00000000, 0b11000000	/* IPC Config: 4clk wait state on ipc */

	.byte	0x52, 0b01110000, 0b00000000	// PCI IRQ A disable
	.byte	0x53, 0b01110000, 0b00000000	// PCI IRQ B disable
	.byte	0x54, 0b01110000, 0b00000000	// PCI IRQ C disable
	.byte	0x55, 0b01110000, 0b00000000	// PCI IRQ D disable
 	.byte	0x56, 0b00000111, 0b00000000	/* IRQ Level: irqs 0-7 are edge sensitive */
 	.byte	0x57, 0b00100000, 0b00000001	/* IRQ Level: irqs 8-15 are edge sensitive, ISA refresh toggle. */
	.byte	0x58, 0b01110000, 0b00000000	// VMI IRQ routing: disable


	.byte	0x40, 0b10000000, 0x4b			// m=9, n=166, p=3
	.byte	0x41, 0b00000000, 0xa6			// f= 66 Mhz
 /*
  * 	Graphic stuff:
  */
	.byte	0x29, 0b11110000, 0b00000011	/* VGA enable, PCI palette snoop disabled*/
/*
 *	Power managment:
 *    disable all
 */
	.byte	0x60, 0b00000011, 0b00000000
	.byte	0x61, 0b10000001, 0b00000000
	.byte	0x8d, 0b00011111, 0b00000000
	.byte	0x62, 0b00000001, 0b00000000
	.byte	0x63, 0b11011111, 0b00000000
	.byte	0x64, 0b00011111, 0b00000000
	.byte	0x65, 0b00000011, 0b00000000
	.byte	0x66, 0b11011111, 0b00000000
	.byte	0x67, 0b00000011, 0b00000000
	.byte	0x69, 0b00000011, 0b00000000
//	.byte	0x68, 0b00000000, 0b00000000 // doesnt exist??
//	.byte	0x6a, 0b00000000, 0b00000000 // not necessary
	.byte	0x6b, 0b00000000, 0b00000000 // PMU Range00
	.byte	0x6c, 0b00000000, 0b00000000 // PMU Range01

/*
 *	SMI generation:
 */
	.byte	0x71, 0b00000001, 0b00000000	/* do not generate SMI */
	
	/* PMU: */
	.byte	0x7a, 0b10000000, 0b00000000	// PMU Status Reg
	.byte	0x7c, 0b00000001, 0b00000000	/* CPU clock trottle disable,
											 * Clock off enable */


	.byte	0xff				/* end of table */


/*
 *
 */
stpc_dram_refresh:
// the clock freq table from the Bios Writers Guide conflicts with that in the data sheet
// for clock speeds, so the data sheet values are used.
// empty positions are filled in with defaults at 66 Mhz
//
	/* frequencies in Mhz */
	/*
	clock freq from BWG based on reading strap value; data sheet differs
	.byte 25, 50, 60, 66, 135/2, 69, 141/2, 72, 147/2, 75, 153/2
	.byte 78, 159/2, 81, 165/2, 84, 171/2, 87, 177/2, 90, 183/2, 93
	.byte 189/2, 96, 195/2, 100, 105, 110, 115, 120, 125, 133
	*/

	.byte 25*156/(10*16), 50*156/(10*16), 60*156/(10*16), 66*156/(10*16), 0x41,0x41,0x41,0x41
	.byte 0x41, 75*156/(10*16), 0x41,0x41,0x41,0x41,0x41,0x41
	.byte 0x41,0x41,0x41,90*156/(10*16),0x41,0x41,0x41,0x41
	.byte 0x41, 100*156/(10*16),0x41,0x41,0x41,0x41,0x41,0x41

stpc_chip0:
