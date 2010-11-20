/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Nick Barker <nick.barker9@btinternet.com>
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
  Automatically detect and set up ddr dram on the CLE266 chipset.
  Assumes DDR memory, though chipset also supports SDRAM
  Assumes at least 266Mhz memory as no attempt is made to clock
  the chipset down if slower memory is installed.
  So far tested on:
	256 Mb 266Mhz 1 Bank (i.e. single sided)
	256 Mb 266Mhz 2 Bank (i.e. double sided)
	512 Mb 266Mhz 2 Bank (i.e. double sided)
*/
/* ported and enhanced from assembler level code in coreboot v1 */

#include <spd.h>
#include <cpu/x86/mtrr.h>
#include "raminit.h"



void dimm_read(unsigned long bank,unsigned long x)
{
	//unsigned long eax;
	volatile unsigned long y;
	//eax =  x;
	y = * (volatile unsigned long *) (x+ bank) ;

}


void
dumpnorth(device_t north)
{
	uint16_t r, c;
	for(r = 0; r < 256; r += 16) {
		print_debug_hex8(r);
		print_debug(":");
		for(c = 0; c < 16; c++) {
			print_debug_hex8(pci_read_config8(north, r+c));
			print_debug(" ");
		}
		print_debug("\n");
  }
}
void print_val(char *str, int val)
{
	print_debug(str);
	print_debug_hex8(val);
}

static void ddr_ram_setup(const struct mem_controller *ctrl)
{
	device_t north = (device_t) 0;
	uint8_t b, c, bank;
	uint16_t i;
	unsigned long bank_address;

	print_debug("vt8623 init starting\n");
	north = pci_locate_device(PCI_ID(0x1106, 0x3123), 0);
	north = 0;


	pci_write_config8(north,0x75,0x08);

	/* setup cpu */
	pci_write_config8(north,0x50,0xc8);
	pci_write_config8(north,0x51,0xde);
	pci_write_config8(north,0x52,0xcf);
	pci_write_config8(north,0x53,0x88);
	pci_write_config8(north,0x55,0x04);

/*
    DRAM MA Map Type  Device 0  Offset 58

    Determine memory addressing based on the module's memory technology and
    arrangement.  See Table 4-9 of Intel's 82443GX datasheet for details.

    Bank 1/0 MA map type   58[7-5]
    Bank 1/0 command rate  58[4]
    Bank 3/2 MA map type   58[3-1]
    Bank 3/2 command rate  58[0]


    Read SPD byte 17, Number of banks on SDRAM device.
*/
	c = 0;
	b = smbus_read_byte(DIMM0,17);
	print_val("Detecting Memory\nNumber of Banks ",b);

	if( b != 2 ){            // not 16 Mb type

/*
    Read SPD byte 3, Number of row addresses.
*/
		b = smbus_read_byte(DIMM0,3);
		print_val("\nNumber of Rows ",b);
		if( b >= 0x0d ){	// not 64/128Mb (rows <=12)

/*
    Read SPD byte 13, Primary DRAM width.
*/
			b = smbus_read_byte(DIMM0,13);
			print_val("\nPriamry DRAM width",b);
			if( b != 4 )   // mot 64/128Mb (x4)
				c = 0x80;  // 256Mb
		}

/*
    64/128Mb chip

    Read SPD byte 4, Number of column addresses.
*/
		b = smbus_read_byte(DIMM0,4);
		print_val("\nNo Columns ",b);
		if( b == 10 || b == 11 ) c |= 0x60;   // 10/11 bit col addr
		if( b == 9 ) c |= 0x40;           // 9 bit col addr
		if( b == 8 ) c |= 0x20;           // 8 bit col addr

	}
	print_val("\nMA type ",c);
	pci_write_config8(north,0x58,c);

/*
    DRAM bank size.  See 4.3.1 pg 35

    5a->5d  set to end address for each bank.  1 bit == 16MB
    5a = bank 0
    5b = bank 0 + b1
    5c = bank 0 + b1 + b2
    5d = bank 0 + b1 + b2 + b3
*/

// Read SPD byte 31 Module bank density
	c = 0;
	b = smbus_read_byte(DIMM0,31);
	if( b & 0x02 ) c = 0x80;         // 2GB
	else if( b & 0x01) c = 0x40;     // 1GB
	else if( b & 0x80) c = 0x20;     // 512Mb
	else if( b & 0x40) c = 0x10;     // 256Mb
	else if( b & 0x20) c = 0x08;     // 128Mb
	else if( b & 0x10) c = 0x04;     // 64Mb
	else if( b & 0x08) c = 0x02;     // 32Mb
	else if( b & 0x04) c = 0x01;     // 16Mb / 4Gb
	else c = 0x01;                   // Error, use default


	print_val("\nBank 0 (*16 Mb) ",c);

	// set bank zero size
	pci_write_config8(north,0x5a,c);
	// SPD byte 5  # of physical banks
	b = smbus_read_byte(DIMM0,5);

	print_val("\nNo Physical Banks ",b);
	if( b == 2)
		c <<=1;

	print_val("\nTotal Memory (*16 Mb) ",c);
	// set banks 1,2,3
	pci_write_config8(north,0x5b,c);
	pci_write_config8(north,0x5c,c);
	pci_write_config8(north,0x5d,c);


	/* Read SPD byte 18 CAS Latency */
	b = smbus_read_byte(DIMM0,18);
	print_debug("\nCAS Supported ");
	if(b & 0x04)
		print_debug("2 ");
	if(b & 0x08)
		print_debug("2.5 ");
	if(b & 0x10)
		print_debug("3");
	print_val("\nCycle time at CL X     (nS)",smbus_read_byte(DIMM0,9));
	print_val("\nCycle time at CL X-0.5 (nS)",smbus_read_byte(DIMM0,23));
	print_val("\nCycle time at CL X-1   (nS)",smbus_read_byte(DIMM0,25));


	if( b & 0x10 ){             // DDR offering optional CAS 3
		print_debug("\nStarting at CAS 3");
		c = 0x30;
		/* see if we can better it */
		if( b & 0x08 ){     // DDR mandatory CAS 2.5
			if( smbus_read_byte(DIMM0,23) <= 0x75 ){ // we can manage 133Mhz at CAS 2.5
				print_debug("\nWe can do CAS 2.5");
				c = 0x20;
			}
		}
		if( b & 0x04 ){     // DDR mandatory CAS 2
			if( smbus_read_byte(DIMM0,25) <= 0x75 ){ // we can manage 133Mhz at CAS 2
				print_debug("\nWe can do CAS 2");
				c = 0x10;
			}
		}
	}else{                     // no optional CAS values just 2 & 2.5
		print_debug("\nStarting at CAS 2.5");
		c = 0x20;          // assume CAS 2.5
		if( b & 0x04){      // Should always happen
			if( smbus_read_byte(DIMM0,23) <= 0x75){ // we can manage 133Mhz at CAS 2
				print_debug("\nWe can do CAS 2");
				c = 0x10;
			}
		}
	}



/*
    DRAM Timing  Device 0  Offset 64

    Row pre-charge  64[7]
    RAS Pulse width 64[6]
    CAS Latency     64[5,4]

         SDR  DDR
      00  1T   -
      01  2T   2T
      10  3T   2.5T
      11  -    3T

    RAS/CAS delay   64[2]
    Bank Interleave 64[1,0]


    Determine row pre-charge time (tRP)

    T    nS    SPD*4   SPD
    1T   7.5   0x1e
    2T   15    0x3c
    3T   22.5  0x5a
    4T   30            0x1e
    5T   37.5          0x25 .5?
    6T   45            0x2d


    Read SPD byte 27, min row pre-charge time.
*/

	b = smbus_read_byte(DIMM0,27);
	print_val("\ntRP ",b);
	if( b > 0x3c )           // set tRP = 3T
		c |= 0x80;


/*
    Determine RAS to CAS delay (tRCD)

    Read SPD byte 29, min row pre-charge time.
*/

	b = smbus_read_byte(DIMM0,29);
	print_val("\ntRCD ",b);
	if( b > 0x3c )           // set tRCD = 3T
		c |= 0x04;

/*
    Determine RAS pulse width (tRAS)


    Read SPD byte 30, device min active to pre-charge time.
*/

	b = smbus_read_byte(DIMM0,30);
	print_val("\ntRAS ",b);
	if( b > 0x25 )           // set tRAS = 6T
		c |= 0x40;


/*
    Determine bank interleave

    Read SPD byte 17, Number of banks on SDRAM device.
*/
	b = smbus_read_byte(DIMM0,17);
	if( b == 4) c |= 0x02;
	else if (b == 2) c |= 0x01;


	/* set DRAM timing for all banks */
	pci_write_config8(north,0x64,c);

	/* set DRAM type to DDR */
	pci_write_config8(north,0x60,0x02);


	/* DRAM arbitration timer */
	pci_write_config8(north,0x65,0x32);


/*
    CPU Frequency  Device 0 Offset 54

    CPU Frequency          54[7,6]  bootstraps at 0xc0 (133Mhz)
    DRAM burst length = 8  54[5]
*/
	pci_write_config8(north,0x54,0xe0);


/*
    DRAM Clock  Device 0 Offset 69

    DRAM/CPU speed      69[7,6]  (leave at default 00 == CPU)
    Controller que > 2  69[5]
    Controller que != 4 69[4]
    DRAM 8k page size   69[3]
    DRAM 4k page size   69[2]
    Multiple page mode  69[0]
*/

	pci_write_config8(north,0x69,0x2d);

	/* Delay >= 100ns after DRAM Frequency adjust, See 4.1.1.3 pg 15 */
	udelay(200);


	/* Enable CKE */
	pci_write_config8(north,0x6b,0x10);
	udelay(200);

	/* Disable DRAM refresh */
	pci_write_config8(north,0x6a,0x0);


	/* Set drive for 1 bank DDR  (Table 4.4.2, pg 40) */
	pci_write_config8(north,0x6d,0x044);
	pci_write_config8(north,0x67,0x3a);

	b = smbus_read_byte(DIMM0,5); // SPD byte 5  # of physical banks
	if( b > 1) {
                // Increase drive control when there is more than 1 physical bank
		pci_write_config8(north,0x6c,0x84);   // Drive control: MA, DQS, MD/CKE
		pci_write_config8(north,0x6d,0x55);   // DC: Early clock select, DQM, CS#, MD
	}
	/* place frame buffer on last bank */
	if( !b) b++;     // make sure at least 1 bank reported
	pci_write_config8(north,0xe3,b-1);

	for( bank = 0 , bank_address=0; bank < b ; bank++){
/*
    DDR init described in Via BIOS Porting Guide.  Pg 28 (4.2.3.1)
*/


		/* NOP command enable */
		pci_write_config8(north,0x6b,0x11);

		/* read a double word from any address of the dimm */
		dimm_read(bank_address,0x1f000);
		//udelay(200);

		/* All bank precharge Command Enable */
		pci_write_config8(north,0x6b,0x12);
		dimm_read(bank_address,0x1f000);


		/* MSR Enable */
		pci_write_config8(north,0x6b,0x13);
		dimm_read(bank_address,0x2000);
		udelay(1);
		dimm_read(bank_address,0x800);
		udelay(1);

		/* All banks precharge Command Enable */
		pci_write_config8(north,0x6b,0x12);
		dimm_read(bank_address,0x1f200);

		/* CBR Cycle Enable */
		pci_write_config8(north,0x6b,0x14);

		/* Read 8 times */
		dimm_read(bank_address,0x1f300);
		udelay(100);
		dimm_read(bank_address,0x1f400);
		udelay(100);
		dimm_read(bank_address,0x1f500);
		udelay(100);
		dimm_read(bank_address,0x1f600);
		udelay(100);
		dimm_read(bank_address,0x1f700);
		udelay(100);
		dimm_read(bank_address,0x1f800);
		udelay(100);
		dimm_read(bank_address,0x1f900);
		udelay(100);
		dimm_read(bank_address,0x1fa00);
		udelay(100);

		/* MSR Enable */
		pci_write_config8(north,0x6b,0x13);

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
		c = pci_read_config8(north,0x64);
		if( (c & 0x30) == 0x10 )
			dimm_read(bank_address,0x150);
		else if((c & 0x30) == 0x20 )
			dimm_read(bank_address,0x350);
		else
			dimm_read(bank_address,0x1d0);

		//dimm_read(bank_address,0x350);

		/* Normal SDRAM Mode */
		pci_write_config8(north,0x6b,0x58 );


		bank_address = pci_read_config8(north,0x5a+bank) * 0x1000000;
	} // end of for each bank

	/* Adjust DQS (data strobe output delay). See 4.2.3.2 pg 29 */
	pci_write_config8(north,0x66,0x41);

	/* determine low bond */
	if( b == 2)
		bank_address = pci_read_config8(north,0x5a) * 0x1000000;
	else
		bank_address = 0;

	for(i = 0 ; i < 0x0ff; i++){
		c = i ^ (i>>1);			// convert to gray code
		pci_write_config8(north,0x68,c);
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
 			pci_write_config8(north,0x68,i ^ (i>>1) );

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
		print_val("  High Bond",i);
		c = ((i - c)<<1)/3 +c;
		print_val("  Setting DQS delay",c);
		c = c ^ (c>>1);		// convert to gray code
		pci_write_config8(north,0x68,c);
		pci_write_config8(north,0x68,0x42);
	}else{
		print_debug("Unable to determine low bond - Setting default\n");
		pci_write_config8(north,0x68,0x59);
	}


	pci_write_config8(north,0x66,0x01);
	pci_write_config8(north,0x55,0x07);



/*
    DRAM refresh rate  Device 0 Offset 6a

    Units of 16 DRAM clock cycles.  (See 4.4.1 pg 39)

    Rx69 (DRAM freq)  Rx58 (chip tech)  Rx6a

    133Mhz            64/128Mb          0x86
    133Mhz            256/512Mb         0x43
    100Mhz            64/128Mb          0x65
    100Mhz            256/512Mb         0x32
*/

	b = pci_read_config8(north,0x58);
	if( b < 0x80 )   // 256 tech
		pci_write_config8(north,0x6a,0x86);
	else
		pci_write_config8(north,0x6a,0x43);

	pci_write_config8(north,0x61,0xff);
	//pci_write_config8(north,0x67,0x22);

	/* pci */
	pci_write_config8(north,0x70,0x82);
	pci_write_config8(north,0x73,0x01);
	pci_write_config8(north,0x76,0x50);


	pci_write_config8(north,0x71,0xc8);


	/* graphics aperture base */

	pci_write_config8(north,0x13,0xd0);

	//pci_write_config8(north,0xe1,0xdf);
	//pci_write_config8(north,0xe2,0x42);
	pci_write_config8(north,0xe0,0x00);

	pci_write_config8(north,0x84,0x80);
	pci_write_config16(north,0x80,0x610f);
	pci_write_config32(north,0x88,0x00000002);



	pci_write_config8(north,0xa8,0x04);
	pci_write_config8(north,0xac,0x2f);
	pci_write_config8(north,0xae,0x04);

        print_debug("vt8623 done\n");
	dumpnorth(north);

	print_debug("AGP\n");
	north = pci_locate_device(PCI_ID(0x1106, 0xb091), 0);
	pci_write_config32(north,0x20,0xddf0dc00);
	pci_write_config32(north,0x24,0xdbf0d800);
	pci_write_config8(north,0x3e,0x0c);
	//dumpnorth(north);

	//print_err("VGA\n");
	//north = pci_locate_device(PCI_ID(0x1106, 0x3122), 0);
	//pci_write_config32(north,0x10,0xd8000008);
	//pci_write_config32(north,0x14,0xdc000000);
	//dumpnorth(north);

}
