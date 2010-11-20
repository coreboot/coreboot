/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Nick Barker <nick.barker9@btinternet.com>
 * Copyright (C) 2009 Jon Harrison <bothlyn@blueyonder.co.uk
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
  Automatically detect and set up ddr dram on the CN400 chipset.
  Assumes DDR400 memory as no attempt is made to clock
  the chipset down if slower memory is installed.
  So far tested on:
	512 Mb DDR400 4 Bank / 2 Rank (1GB) (i.e. double sided)
*/
/* ported from Via VT8263 Code*/

#include <spd.h>
#include <delay.h>
#include <cpu/x86/mtrr.h>
#include "cn400.h"

static void dimm_read(unsigned long bank,unsigned long x)
{
	//unsigned long eax;
	volatile unsigned long y;
	//eax =  x;
	y = * (volatile unsigned long *) (x+ bank) ;

}


static void print_val(char *str, int val)
{
	print_debug(str);
	print_debug_hex8(val);
}

/**
 * Configure the bus between the CPU and the northbridge. This might be able to
 * be moved to post-ram code in the future. For the most part, these registers
 * should not be messed around with. These are too complex to explain short of
 * copying the datasheets into the comments, but most of these values are from
 * the BIOS Porting Guide, so they should work on any board. If they don't,
 * try the values from your factory BIOS.
 *
 * TODO: Changing the DRAM frequency doesn't work (hard lockup).
 *
 * @param dev The northbridge's CPU Host Interface (D0F2).
 */
static void c3_cpu_setup(device_t dev)
{
	/* Host bus interface registers (D0F2 0x50-0x67) */
	/* Taken from CN700 and updated from running CN400 */
	uint8_t reg8;

	/* Host Bus I/O Circuit (see datasheet) */
	/* Host Address Pullup/down Driving */
	pci_write_config8(dev, 0x70, 0x33);
	pci_write_config8(dev, 0x71, 0x44);
	pci_write_config8(dev, 0x72, 0x33);
	pci_write_config8(dev, 0x73, 0x44);

	/* Output Delay Stagger Control */
	pci_write_config8(dev, 0x74, 0x70);

	/* AGTL+ I/O Circuit */
	pci_write_config8(dev, 0x75, 0x08);

	/* AGTL+ Compensation Status */
	pci_write_config8(dev, 0x76, 0x74);

	/* AGTL+ Auto Compensation Offest */
	pci_write_config8(dev, 0x77, 0x00);
	pci_write_config8(dev, 0x78, 0x94);

	/* Request phase control */
	pci_write_config8(dev, 0x50, 0xA8);

	/* Line DRDY# Timing Control */
	pci_write_config8(dev, 0x60, 0x00);
	pci_write_config8(dev, 0x61, 0x00);
	pci_write_config8(dev, 0x62, 0x00);

	/* QW DRDY# Timing Control */
	pci_write_config8(dev, 0x63, 0x00);
	pci_write_config8(dev, 0x64, 0x00);
	pci_write_config8(dev, 0x65, 0x00);

	/* Read Line Burst DRDY# Timing Control */
	pci_write_config8(dev, 0x66, 0x00);
	pci_write_config8(dev, 0x67, 0x00);

	/* CPU Interface Control */
	pci_write_config8(dev, 0x51, 0xFE);
	pci_write_config8(dev, 0x52, 0xEF);

	/* Arbitration */
	pci_write_config8(dev, 0x53, 0x88);

	/* Write Policy & Reorder Latecy */
	pci_write_config8(dev, 0x56, 0x00);

	/* Delivery-Trigger Control */
	pci_write_config8(dev, 0x58, 0x00);

	/* IPI Control */
	pci_write_config8(dev, 0x59, 0x30);

	/* CPU Misc Control */
	pci_write_config8(dev, 0x5C, 0x00);

	/* Write Policy */
	pci_write_config8(dev, 0x5d, 0xb2);

	/* Bandwidth Timer */
	pci_write_config8(dev, 0x5e, 0x88);

	/* CPU Miscellaneous Control */
	pci_write_config8(dev, 0x5f, 0xc7);

	/* CPU Miscellaneous Control */
	pci_write_config8(dev, 0x55, 0x28);
	pci_write_config8(dev, 0x57, 0x69);

	/* CPU Host Bus Final Setup */
	reg8 = pci_read_config8(dev, 0x54);
	reg8 |= 0x08;
	pci_write_config8(dev, 0x54, reg8);

}

static void ddr_ram_setup(void)
{
	uint8_t b, c, bank, ma;
	uint16_t i;
	unsigned long bank_address;


	print_debug("CN400 RAM init starting\n");

	pci_write_config8(ctrl.d0f7, 0x75, 0x08);


	/* No  Interleaving or Multi Page */
	pci_write_config8(ctrl.d0f3, 0x69, 0x00);
	pci_write_config8(ctrl.d0f3, 0x6b, 0x10);

/*
    DRAM MA Map Type  Device 0  Fn3 Offset 50-51

    Determine memory addressing based on the module's memory technology and
    arrangement.  See Table 4-9 of Intel's 82443GX datasheet for details.

    Bank 1/0 MA map type   50[7-5]
    Bank 1/0 command rate  50[4]
    Bank 3/2 MA map type   50[3-1]
    Bank 3/2 command rate  50[0]


    Read SPD byte 17, Number of banks on SDRAM device.
*/
	c = 0;
	b = smbus_read_byte(DIMM0, SPD_NUM_BANKS_PER_SDRAM);
	//print_val("Detecting Memory\nNumber of Banks ",b);

	// Only supporting 4 bank chips just now
	if( b == 4 ){
		/* Read SPD byte 3, Number of row addresses. */
		c = 0x01;
		bank = 0x40;
		b = smbus_read_byte(DIMM0, SPD_NUM_ROWS);
		//print_val("\nNumber of Rows ", b);

		if( b >= 0x0d ){	// 256/512Mb

			if (b == 0x0e)
				bank = 0x48;
			else
				bank = 0x44;

			/* Read SPD byte 13, Primary DRAM width. */
			b = smbus_read_byte(DIMM0, SPD_PRIMARY_SDRAM_WIDTH);
			//print_val("\nPrimary DRAM width", b);
			if( b != 4 )   // not 64/128Mb (x4)
				c = 0x81;  // 256Mb
		}

		/* Read SPD byte 4, Number of column addresses. */
		b = smbus_read_byte(DIMM0, SPD_NUM_COLUMNS);
		//print_val("\nNo Columns ",b);
		if( b == 10 || b == 11 || b == 12) c |= 0x60;   // 10/11 bit col addr
		if( b == 9 ) c |= 0x40;           // 9 bit col addr
		if( b == 8 ) c |= 0x20;           // 8 bit col addr

		//print_val("\nMA type ", c);
		pci_write_config8(ctrl.d0f3, 0x50, c);

	}

	/* Disable Upper Banks */
	pci_write_config8(ctrl.d0f3, 0x51, 0x00);

/*	else
	{
		die("DRAM module size is not supported by CN400\n");
	}
*/

/*
    DRAM bank size.  See 4.3.1 pg 35

    5a->5d  set to end address for each bank.  1 bit == 32MB
    5a = bank 0
    5b = bank 0 + b1
    5c = bank 0 + b1 + b2
    5d = bank 0 + b1 + b2 + b3
*/

	// Read SPD byte 31 Module bank density
	//c = 0;
	b = smbus_read_byte(DIMM0, SPD_DENSITY_OF_EACH_ROW_ON_MODULE);
	if( b & 0x02 )
	{
		c = 0x40;         				// 2GB
		bank |= 0x02;
	}
	else if( b & 0x01)
	{
		c = 0x20;    					// 1GB
		if (bank == 0x48) bank |= 0x01;
		else bank |= 0x03;
	}
	else if( b & 0x80)
	{
		c = 0x10;    					// 512MB
		if (bank == 0x44) bank |= 0x02;
	}
	else if( b & 0x40)
	{
		c = 0x08;    					// 256MB
		if (bank == 0x44) bank |= 0x01;
		else bank |= 0x03;
	}
	else if( b & 0x20)
	{
		c = 0x04;    					// 128MB
		if (bank == 0x40) bank |= 0x02;
	}
	else if( b & 0x10)
	{
		c = 0x02;    					// 64MB
		bank |= 0x01;
	}
	else if( b & 0x08) c = 0x01;    	// 32MB
	else c = 0x01;                  	// Error, use default

	// set bank zero size
	pci_write_config8(ctrl.d0f3, 0x40, c);

	// SPD byte 5  # of physical banks
	b = smbus_read_byte(DIMM0, SPD_NUM_DIMM_BANKS);

	//print_val("\nNo Physical Banks ",b);
	if( b == 2)
	{
		c <<=1;
		bank |= 0x80;
	}
/*	else
	{
		die("Only a single DIMM is supported by EPIA-N(L)\n");
	}
*/
	// set banks 1,2,3...
	pci_write_config8(ctrl.d0f3, 0x41,c);
	pci_write_config8(ctrl.d0f3, 0x42,c);
	pci_write_config8(ctrl.d0f3, 0x43,c);
	pci_write_config8(ctrl.d0f3, 0x44,c);
	pci_write_config8(ctrl.d0f3, 0x45,c);
	pci_write_config8(ctrl.d0f3, 0x46,c);
	pci_write_config8(ctrl.d0f3, 0x47,c);

	/* Top Rank Address Mirrored to the South Bridge */
	/* over the VLink								 */
	pci_write_config8(ctrl.d0f7, 0x57, (c << 1));

	ma = bank;

	/* Read SPD byte 18 CAS Latency */
	b = smbus_read_byte(DIMM0, SPD_ACCEPTABLE_CAS_LATENCIES);
/*	print_debug("\nCAS Supported ");
	if(b & 0x04)
		print_debug("2 ");
	if(b & 0x08)
		print_debug("2.5 ");
	if(b & 0x10)
		print_debug("3");

	c = smbus_read_byte(DIMM0, SPD_MIN_CYCLE_TIME_AT_CAS_MAX);
	print_val("\nCycle time at CL X     (nS)", c);
	c = smbus_read_byte(DIMM0, SPD_SDRAM_CYCLE_TIME_2ND);
	print_val("\nCycle time at CL X-0.5 (nS)", c);
	c = smbus_read_byte(DIMM0, SPD_SDRAM_CYCLE_TIME_3RD);
	print_val("\nCycle time at CL X-1   (nS)", c);
*/
	/* Scaling of Cycle Time SPD data */
	/* 7      4 3       0             */
	/*    ns     x0.1ns               */
	bank = smbus_read_byte(DIMM0, SPD_MIN_CYCLE_TIME_AT_CAS_MAX);

	if( b & 0x10 ){             // DDR offering optional CAS 3
		//print_debug("\nStarting at CAS 3");
		c = 0x30;
		/* see if we can better it */
		if( b & 0x08 ){     // DDR mandatory CAS 2.5
			if( smbus_read_byte(DIMM0, SPD_SDRAM_CYCLE_TIME_2ND) <= bank ){ // we can manage max MHz at CAS 2.5
				//print_debug("\nWe can do CAS 2.5");
				c = 0x20;
			}
		}
		if( b & 0x04 ){     // DDR mandatory CAS 2
			if( smbus_read_byte(DIMM0, SPD_SDRAM_CYCLE_TIME_3RD) <= bank ){ // we can manage max Mhz at CAS 2
				//print_debug("\nWe can do CAS 2");
				c = 0x10;
			}
		}
	}else{                     // no optional CAS values just 2 & 2.5
		//print_debug("\nStarting at CAS 2.5");
		c = 0x20;          // assume CAS 2.5
		if( b & 0x04){      // Should always happen
			if( smbus_read_byte(DIMM0, SPD_SDRAM_CYCLE_TIME_2ND) <= bank){ // we can manage max Mhz at CAS 2
				//print_debug("\nWe can do CAS 2");
				c = 0x10;
			}
		}
	}

	/* Scale DRAM Cycle Time to tRP/tRCD */
	/* 7      2 1       0             */
	/*    ns     x0.25ns               */
	if ( bank <= 0x50 ) bank = 0x14;
	else if (bank <= 0x60) bank = 0x18;
	else bank = 0x1E;

/*
    DRAM Timing  Device 0  Fn 3 Offset 56

    RAS Pulse width 56[7,6]
    CAS Latency     56[5,4]
    Row pre-charge  56[1,0]

         SDR  DDR
      00  1T   -
      01  2T   2T
      10  3T   2.5T
      11  -    3T

    RAS/CAS delay   56[3,2]

    Determine row pre-charge time (tRP)


    Read SPD byte 27, min row pre-charge time.
*/

	b = smbus_read_byte(DIMM0, SPD_MIN_ROW_PRECHARGE_TIME);

	//print_val("\ntRP ",b);
	if ( b >= (5 * bank)) {
		c |= 0x03;		// set tRP = 5T
	}
	else if ( b >= (4 * bank)) {
		c |= 0x02;		// set tRP = 4T
	}
	else if ( b >= (3 * bank)) {
		c |= 0x01;		// set tRP = 3T
	}

/*
    Determine RAS to CAS delay (tRCD)

    Read SPD byte 29, min row pre-charge time.
*/

	b = smbus_read_byte(DIMM0, SPD_MIN_RAS_TO_CAS_DELAY);
	//print_val("\ntRCD ",b);

	if ( b >= (5 * bank)) c |= 0x0C;		// set tRCD = 5T
	else if ( b >= (4 * bank)) c |= 0x08;	// set tRCD = 4T
	else if ( b >= (3 * bank)) c |= 0x04;	// set tRCD = 3T

/*
    Determine RAS pulse width (tRAS)


    Read SPD byte 30, device min active to pre-charge time.
*/

	/* tRAS is in whole ns */
	bank = bank >> 2;

	b = smbus_read_byte(DIMM0, SPD_MIN_ACTIVE_TO_PRECHARGE_DELAY);
	//print_val("\ntRAS ",b);
	//print_val("\nBank ", bank);
	if ( b >= (9 * bank)) c |= 0xC0;		// set tRAS = 9T
	else if ( b >= (8 * bank)) c |= 0x80;	// set tRAS = 8T
	else if ( b >= (7 * bank)) c |= 0x40;	// set tRAS = 7T

	/* Write DRAM Timing All Banks I */
	pci_write_config8(ctrl.d0f3, 0x56, c);

	/* TWrite DRAM Timing All Banks II */
	pci_write_config8(ctrl.d0f3, 0x57, 0x1a);

	/* DRAM arbitration timer */
	pci_write_config8(ctrl.d0f3, 0x65, 0x99);

/*
    DRAM Clock  Device 0 Fn 3 Offset 68
*/
	bank = smbus_read_byte(DIMM0, SPD_MIN_CYCLE_TIME_AT_CAS_MAX);

	/* Setup DRAM Cycle Time */
	if ( bank <= 0x50 )
	{
		/* DRAM DDR Control Alert! Alert! See also c3_cpu_setup */
		/* This sets to 133MHz FSB / DDR400. */
		pci_write_config8(ctrl.d0f3, 0x68, 0x85);
	}
	else if (bank <= 0x60)
	{
		/* DRAM DDR Control Alert! Alert! This hardwires to */
		/* 133MHz FSB / DDR333.  See also c3_cpu_setup */
		pci_write_config8(ctrl.d0f3, 0x68, 0x81);
	}
	else
	{
		/* DRAM DDR Control Alert! Alert! This hardwires to */
		/* 133MHz FSB / DDR266.  See also c3_cpu_setup */
		pci_write_config8(ctrl.d0f3, 0x68, 0x80);
	}

	/* Delay >= 100ns after DRAM Frequency adjust, See 4.1.1.3 pg 15 */
	udelay(200);

/*
    Determine bank interleave

    Read SPD byte 17, Number of banks on SDRAM device.
*/
	c = 0x0F;
	b = smbus_read_byte(DIMM0, SPD_NUM_BANKS_PER_SDRAM);
	if( b == 4) c |= 0x80;
	else if (b == 2) c |= 0x40;

	/* 4-Way Interleave With Multi-Paging (From Running System)*/
	pci_write_config8(ctrl.d0f3, 0x69, c);

	/*DRAM Controller Internal Options */
	pci_write_config8(ctrl.d0f3, 0x54, 0x01);

	/* DRAM Arbitration Control */
	pci_write_config8(ctrl.d0f3, 0x66, 0x82);

	/* DRAM Control */
	pci_write_config8(ctrl.d0f3, 0x6e, 0x80);

	/* Disable refresh for now */
	pci_write_config8(ctrl.d0f3, 0x6a, 0x00);

	/* DDR Clock Gen Duty Cycle Control */
	pci_write_config8(ctrl.d0f3, 0xEE, 0x01);


	/* DRAM Clock Control */
	pci_write_config8(ctrl.d0f3, 0x6c, 0x00);

	/* DRAM Bus Turn-Around Setting */
	pci_write_config8(ctrl.d0f3, 0x60, 0x01);

	/* Disable DRAM refresh */
	pci_write_config8(ctrl.d0f3,0x6a,0x0);


	/* Memory Pads Driving and Range Select */
	pci_write_config8(ctrl.d0f3, 0xe2, 0xAA);
	pci_write_config8(ctrl.d0f3, 0xe3, 0x00);
	pci_write_config8(ctrl.d0f3, 0xe4, 0x99);

	/* DRAM signal timing control */
	pci_write_config8(ctrl.d0f3, 0x74, 0x99);
	pci_write_config8(ctrl.d0f3, 0x76, 0x09);
	pci_write_config8(ctrl.d0f3, 0x77, 0x12);

	pci_write_config8(ctrl.d0f3, 0xe0, 0xAA);
	pci_write_config8(ctrl.d0f3, 0xe1, 0x00);
	pci_write_config8(ctrl.d0f3, 0xe6, 0x00);
	pci_write_config8(ctrl.d0f3, 0xe8, 0xEE);
	pci_write_config8(ctrl.d0f3, 0xea, 0xEE);


	/* SPD byte 5  # of physical banks */
	b = smbus_read_byte(DIMM0, SPD_NUM_DIMM_BANKS) -1;
	c = b | 0x40;

	pci_write_config8(ctrl.d0f3, 0xb0, c);

	/* Set RAM Decode method */
	pci_write_config8(ctrl.d0f3, 0x55, 0x0a);

	/* Enable DIMM Ranks */
	pci_write_config8(ctrl.d0f3, 0x48, ma);
	udelay(200);

	c = smbus_read_byte(DIMM0, SPD_SUPPORTED_BURST_LENGTHS);
	c &= 0x08;
	if ( c == 0x08 )
	{
		print_debug("Setting Burst Length 8\n");
		/*
    		CPU Frequency  Device 0 Function 2 Offset 54

			CPU FSB Operating Frequency (bits 7:5)
	    	  	000 : 100MHz    001 : 133MHz
	    	  	010 : 200MHz
			  	011->111 : Reserved

			SDRAM BL8 (4)

			Don't change Frequency from power up defaults
			This seems to lockup the RAM interface
		*/
		c = pci_read_config8(ctrl.d0f2, 0x54);
		c |= 0x10;
		pci_write_config8(ctrl.d0f2, 0x54, c);
		i = 0x008; 		// Used later to set SDRAM MSR
	}


	for( bank = 0 , bank_address=0; bank <= b ; bank++) {
/*
    DDR init described in Via VT8623 BIOS Porting Guide.  Pg 28 (4.2.3.1)
*/

		/* NOP command enable */
		c = pci_read_config8(ctrl.d0f3, DRAM_MISC_CTL);
		c &= 0xf8;		/* Clear bits 2-0. */
		c |= RAM_COMMAND_NOP;
		pci_write_config8(ctrl.d0f3, DRAM_MISC_CTL, c);

		/* read a double word from any address of the dimm */
		dimm_read(bank_address,0x1f000);
		//udelay(200);

		/* All bank precharge Command Enable */
		c = pci_read_config8(ctrl.d0f3, DRAM_MISC_CTL);
		c &= 0xf8;		/* Clear bits 2-0. */
		c |= RAM_COMMAND_PRECHARGE;
		pci_write_config8(ctrl.d0f3, DRAM_MISC_CTL, c);
		dimm_read(bank_address,0x1f000);


		/* MSR Enable Low DIMM*/
		c = pci_read_config8(ctrl.d0f3, DRAM_MISC_CTL);
		c &= 0xf8;		/* Clear bits 2-0. */
		c |= RAM_COMMAND_MSR_LOW;
		pci_write_config8(ctrl.d0f3, DRAM_MISC_CTL, c);
		/* TODO: Bank Addressing for Different Numbers of Row Addresses */
		dimm_read(bank_address,0x2000);
		udelay(1);
		dimm_read(bank_address,0x800);
		udelay(1);

		/* All banks precharge Command Enable */
		c = pci_read_config8(ctrl.d0f3, DRAM_MISC_CTL);
		c &= 0xf8;		/* Clear bits 2-0. */
		c |= RAM_COMMAND_PRECHARGE;
		pci_write_config8(ctrl.d0f3, DRAM_MISC_CTL, c);
		dimm_read(bank_address,0x1f200);

		/* CBR Cycle Enable */
		c = pci_read_config8(ctrl.d0f3, DRAM_MISC_CTL);
		c &= 0xf8;		/* Clear bits 2-0. */
		c |= RAM_COMMAND_CBR;
		pci_write_config8(ctrl.d0f3, DRAM_MISC_CTL, c);

		/* Read 8 times */
		for (c=0;c<8;c++) {
			dimm_read(bank_address,0x1f300);
			udelay(100);
		}

		/* MSR Enable */
		c = pci_read_config8(ctrl.d0f3, DRAM_MISC_CTL);
		c &= 0xf8;		/* Clear bits 2-0. */
		c |= RAM_COMMAND_MSR_LOW;
		pci_write_config8(ctrl.d0f3, DRAM_MISC_CTL, c);


/*
    Mode Register Definition
    with adjustement so that address calculation is correct - 64 bit technology, therefore
    a0-a2 refer to byte within a 64 bit long word, and a3 is the first address line presented
    to DIMM as a row or column address.

    MR[9-7]   CAS Latency
    MR[6]     Burst Type 0 = sequential, 1 = interleaved
    MR[5-3]   burst length 001 = 2, 010 = 4, 011 = 8, others reserved
    MR[0-2]   dont care

    CAS Latency
    000       reserved
    001       reserved
    010       2
    011       3
    100       reserved
    101       1.5
    110       2.5
    111       reserved

    CAS 2     0101011000 = 0x158
    CAS 2.5   1101011000 = 0x358
    CAS 3     0111011000 = 0x1d8

*/
		c = pci_read_config8(ctrl.d0f3, 0x56);
		if( (c & 0x30) == 0x10 )
			dimm_read(bank_address,(0x150 + i));
		else if((c & 0x30) == 0x20 )
			dimm_read(bank_address,(0x350 + i));
		else
			dimm_read(bank_address,(0x1d0 + i));


		/* Normal SDRAM Mode */
		c = pci_read_config8(ctrl.d0f3, DRAM_MISC_CTL);
		c &= 0xf8;		/* Clear bits 2-0. */
		c |= RAM_COMMAND_NORMAL;
		pci_write_config8(ctrl.d0f3, DRAM_MISC_CTL, c);

		bank_address = pci_read_config8(ctrl.d0f3,0x40+bank) * 0x2000000;
	} // end of for each bank


	/* Set DRAM DQS Output Control */
	pci_write_config8(ctrl.d0f3, 0x79, 0x11);

	/* Set DQS A/B Input delay to defaults */
	pci_write_config8(ctrl.d0f3, 0x7A, 0xA1);
	pci_write_config8(ctrl.d0f3, 0x7B, 0x62);

	/* DQS Duty Cycle Control */
	pci_write_config8(ctrl.d0f3, 0xED, 0x11);

	/* SPD byte 5  # of physical banks */
	b = smbus_read_byte(DIMM0, SPD_NUM_DIMM_BANKS) -1;

	/* determine low bond */
	if( b == 2)
		bank_address = pci_read_config8(ctrl.d0f3,0x40) * 0x2000000;
	else
		bank_address = 0;

	for(i = 0x30 ; i < 0x0ff; i++){
		pci_write_config8(ctrl.d0f3,0x70,i);
		// clear
		*(volatile unsigned long*)(0x4000) = 0;
		*(volatile unsigned long*)(0x4100+bank_address) = 0;
		*(volatile unsigned long*)(0x4200) = 0;
		*(volatile unsigned long*)(0x4300+bank_address) = 0;
		*(volatile unsigned long*)(0x4400) = 0;
		*(volatile unsigned long*)(0x4500+bank_address) = 0;

		// fill
		*(volatile unsigned long*)(0x4000) = 0x12345678;
		*(volatile unsigned long*)(0x4100+bank_address) = 0x81234567;
		*(volatile unsigned long*)(0x4200) = 0x78123456;
		*(volatile unsigned long*)(0x4300+bank_address) = 0x67812345;
		*(volatile unsigned long*)(0x4400) = 0x56781234;
		*(volatile unsigned long*)(0x4500+bank_address) = 0x45678123;

			// verify
		if( *(volatile unsigned long*)(0x4000) != 0x12345678)
			continue;

		if( *(volatile unsigned long*)(0x4100+bank_address) != 0x81234567)
			continue;

		if( *(volatile unsigned long*)(0x4200) != 0x78123456)
			continue;

		if( *(volatile unsigned long*)(0x4300+bank_address) != 0x67812345)
			continue;

		if( *(volatile unsigned long*)(0x4400) != 0x56781234)
			continue;

		if( *(volatile unsigned long*)(0x4500+bank_address) != 0x45678123)
			continue;

		// if everything verified then found low bond
		break;

	}
	print_val("\nLow Bond ",i);
	if( i < 0xff ){
		c = i++;
		for(  ; i <0xff ; i++){
			pci_write_config8(ctrl.d0f3,0x70, i);
			// clear
			*(volatile unsigned long*)(0x8000) = 0;
			*(volatile unsigned long*)(0x8100+bank_address) = 0;
			*(volatile unsigned long*)(0x8200) = 0x0;
			*(volatile unsigned long*)(0x8300+bank_address) = 0;
			*(volatile unsigned long*)(0x8400) = 0x0;
			*(volatile unsigned long*)(0x8500+bank_address) = 0;

			// fill
			*(volatile unsigned long*)(0x8000) = 0x12345678;
			*(volatile unsigned long*)(0x8100+bank_address) = 0x81234567;
			*(volatile unsigned long*)(0x8200) = 0x78123456;
			*(volatile unsigned long*)(0x8300+bank_address) = 0x67812345;
			*(volatile unsigned long*)(0x8400) = 0x56781234;
			*(volatile unsigned long*)(0x8500+bank_address) = 0x45678123;

			// verify
			if( *(volatile unsigned long*)(0x8000) != 0x12345678)
				break;

			if( *(volatile unsigned long*)(0x8100+bank_address) != 0x81234567)
				break;

			if( *(volatile unsigned long*)(0x8200) != 0x78123456)
				break;

			if( *(volatile unsigned long*)(0x8300+bank_address) != 0x67812345)
				break;

			if( *(volatile unsigned long*)(0x8400) != 0x56781234)
				break;

			if( *(volatile unsigned long*)(0x8500+bank_address) != 0x45678123)
				break;

		}
		print_val("  High Bond ",i);
		c = ((i - c)<<1)/3 + c;
		print_val("  Setting DQS delay",c);
		print_debug("\n");
		pci_write_config8(ctrl.d0f3,0x70,c);
	}else{
		pci_write_config8(ctrl.d0f3,0x70,0x67);
	}

	/* Set DQS ChA Data Output Delay to the default */
	pci_write_config8(ctrl.d0f3, 0x71, 0x65);

	/* Set Ch B DQS Output Delays */
	pci_write_config8(ctrl.d0f3, 0x72, 0x2a);
	pci_write_config8(ctrl.d0f3, 0x73, 0x29);

	pci_write_config8(ctrl.d0f3, 0x78, 0x03);

	/* Mystery Value */
	pci_write_config8(ctrl.d0f3, 0x67, 0x50);

	/* Enable Toggle Limiting */
	pci_write_config8(ctrl.d0f4, 0xA3, 0x80);

/*
    DRAM refresh rate  Device 0 F3 Offset 6a
	TODO :: Fix for different DRAM technologies
	other than 512Mb and DRAM Freq
    Units of 16 DRAM clock cycles - 1.
*/
	//c = pci_read_config8(ctrl.d0f3, 0x68);
	//c &= 0x07;
	//b = smbus_read_byte(DIMM0, SPD_REFRESH);
	//print_val("SPD_REFRESH = ", b);

	pci_write_config8(ctrl.d0f3,0x6a,0x65);

	/* SMM and APIC decoding, we do not use SMM */
	b = 0x29;
	pci_write_config8(ctrl.d0f3, 0x86, b);
	/* SMM and APIC decoding mirror */
	pci_write_config8(ctrl.d0f7, 0xe6, b);

	/* Open Up the Rest of the Shadow RAM */
	pci_write_config8(ctrl.d0f3,0x80,0xff);
	pci_write_config8(ctrl.d0f3,0x81,0xff);

	/* pci */
	pci_write_config8(ctrl.d0f7,0x70,0x82);
	pci_write_config8(ctrl.d0f7,0x73,0x01);
	pci_write_config8(ctrl.d0f7,0x76,0x50);

	pci_write_config8(ctrl.d0f7,0x71,0xc8);


	/* VGA device. */
	pci_write_config16(ctrl.d0f3, 0xa0, (1 << 15));
	pci_write_config16(ctrl.d0f3, 0xa4, 0x0010);
    print_debug("CN400 raminit.c done\n");
}
