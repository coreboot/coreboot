#include <cpu/p6/mtrr.h>
#include "raminit.h"

/*
This software and ancillary information (herein called SOFTWARE )
called LinuxBIOS          is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
been authored by an employee or employees of the University of
California, operator of the Los Alamos National Laboratory under
Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
U.S. Government has rights to use, reproduce, and distribute this
SOFTWARE.  The public may copy, distribute, prepare derivative works
and publicly display this SOFTWARE without charge, provided that this
Notice and any statement of authorship are reproduced on all copies.
Neither the Government nor the University makes any warranty, express 
or implied, or assumes any liability or responsibility for the use of
this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
such modified SOFTWARE should be clearly marked, so as not to confuse
it with the version available from LANL.
 */
/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 */
/*
 * 11/26/02 - kevinh@ispiri.com - The existing comments implied that
 * this didn't work yet.  Therefore, I've updated it so that it works
 * correctly - at least on my VIA epia motherboard.  64MB DIMM in slot 0.
 */

/* Added automatic detection of first equipped bank and its MA mapping type.
 * (Rest of configuration is done in C)
 * 5/19/03 by SONE Takeshi <ts1@tsn.or.jp>
 */
/* converted to C 9/2003 Ron Minnich */

/* Set to 1 if your DIMMs are PC133 Note that I'm assuming CPU's FSB
 * frequency is 133MHz. If your CPU runs at another bus speed, you
 * might need to change some of register values.
 */
#ifndef DIMM_PC133
#define DIMM_PC133 0
#endif

// Set to 1 if your DIMMs are CL=2
#ifndef DIMM_CL2
#define DIMM_CL2 0
#endif





void dimm_read(unsigned long x) 
{
	unsigned long eax; 
	volatile unsigned long y;
	eax =  x;
	y = * (volatile unsigned long *) eax;

}

void dimms_write(int x) 
{
	uint8_t c;
	unsigned long eax = x;
	for(c = 0; c < 6; c++) {
		*(volatile unsigned long *) eax = 0;
		eax += 0x10000000;
	}
}



#ifdef DEBUG_SETNORTHB
void setnorthb(device_t north, uint8_t reg, uint8_t val) 
{
	print_debug("setnorth: reg ");
	print_debug_hex8(reg);
	print_debug(" to ");
	print_debug_hex8(val);
	print_debug("\r\n");
	pci_write_config8(north, reg, val);
}
#else
#define setnorthb pci_write_config8
#endif

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
		print_debug("\r\n");
  }
}

static void sdram_set_registers(const struct mem_controller *ctrl) 
{
	device_t north = (device_t) 0;
	uint8_t c, r;

	print_err("vt8623 init starting\r\n");
	north = pci_locate_device(PCI_ID(0x1106, 0x3123), 0);
	north = 0;
	print_debug_hex32(north);
	print_debug(" is the north\n");
	print_debug_hex16(pci_read_config16(north, 0));
	print_debug(" ");
	print_debug_hex16(pci_read_config16(north, 2));
	print_debug("\r\n");
	
	/* All we are doing now is setting initial known-good values that will
	 * be revised later as we read SPD
	 */	

	pci_write_config8(north,0x75,0x08);

	/* since we only support epia-m at the moment, only ddr is supported */
	/* setup cpu */
	pci_write_config8(north,0x50,0xc8);
	pci_write_config8(north,0x51,0xde);
	pci_write_config8(north,0x52,0xcf);
	pci_write_config8(north,0x53,0x88);
	pci_write_config8(north,0x55,0x07);

	/* DRAM MA Map Type */
	pci_write_config8(north,0x58,0xe0);

	/* DRAM bank 0 - 3 size = 512M */
	pci_write_config8(north,0x5a,0x10);
	pci_write_config8(north,0x5b,0x10);
	pci_write_config8(north,0x5c,0x10);
	pci_write_config8(north,0x5d,0x10);

	/* set DRAM timing for all banks */
	pci_write_config8(north,0x64,0xe6);

	/* set DRAM type to DDR */
	pci_write_config8(north,0x60,0x02);


	/* DRAM arbitration timer */
	pci_write_config8(north,0x65,0x32);
	pci_write_config8(north,0x66,0x01);
	pci_write_config8(north,0x68,0x59);


	/* DRAM Frequency */
	pci_write_config8(north,0x54,0xe0);
	pci_write_config8(north,0x69,0x2d);

	/* Enable CKE */
	pci_write_config8(north,0x6b,0x10);
	
	/* Disable DRAM refresh */
	pci_write_config8(north,0x6a,0x0);

	/* set heavy drive */
	pci_write_config8(north,0x6d,0x44);


	pci_write_config8(north,0x61,0xff);



}

/* slot is the dram slot. Return size of side0 in lower 16-bit,
 * side1 in upper 16-bit, in units of 8MB */
static unsigned long 
spd_module_size(unsigned char slot) 
{ 
	/* for all the DRAMS, see if they are there and get the size of each
	 * module. This is just a very early first cut at sizing.
	 */
	/* we may run out of registers ... */
	unsigned int banks, rows, cols, reg;
	unsigned int value = 0;
	unsigned int module = ((0x50 + slot) << 1) + 1;
	/* is the module there? if byte 2 is not 4, then we'll assume it 
	 * is useless. 
	 */
	print_info("Slot "); 
	print_info_hex8(slot); 
	if (smbus_read_byte(module, 2) != 4) {
		print_info(" is empty\r\n");
		return 0;
	}
	print_info(" is SDRAM ");
	
	banks = smbus_read_byte(module, 17);
	/* we're going to assume symmetric banks. Sorry. */
	cols = smbus_read_byte(module, 4)  & 0xf;
	rows = smbus_read_byte(module, 3)  & 0xf;
	/* grand total. You have rows+cols addressing, * times of banks, times
	 * width of data in bytes */
	/* Width is assumed to be 64 bits == 8 bytes */
	value = (1 << (cols + rows)) * banks * 8;
	print_info_hex32(value);
	print_info(" bytes ");
	/* Return in 8MB units */
	value >>= 23;

	/* We should have single or double side */
	if (smbus_read_byte(module, 5) == 2) {
		print_info("x2");
		value = (value << 16) | value;
	}
	print_info("\r\n");
	return value;

}

static int
spd_num_chips(unsigned char slot) 
{ 
	unsigned int module = ((0x50 + slot) << 1) + 1;
	unsigned int width;

	width = smbus_read_byte(module, 13);
	if (width == 0)
		width = 8;
	return 64 / width;
}

static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
#define T133 7
	unsigned char Trp = 1, Tras = 1, casl = 2, val;
	unsigned char timing = 0xe4;
	/* read Trp */
	val = smbus_read_byte(0xa0, 27);
	if (val < 2*T133)
		Trp = 1;
	val = smbus_read_byte(0xa0, 30);
	if (val < 5*T133)
		Tras = 0;
	val = smbus_read_byte(0xa0, 18);
	if (val < 8)
		casl = 1;
	if (val < 4)
		casl = 0;
	
	val = (Trp << 7) | (Tras << 6) | (casl << 4) | 4;
	
	print_debug_hex8(val); print_debug(" is the computed timing\n");
	/* don't set it. Experience shows that this screwy chipset should just
	 * be run with the most conservative timing.
	 * pci_write_config8(0, 0x64, val);
	 */
}

static void set_ma_mapping(device_t north, int slot, int type)
{
    unsigned char reg, val;
    int shift;

    reg = 0x58 + slot/2;
    if (slot%2 >= 1)
        shift = 0;
    else
        shift = 4;

    val = pci_read_config8(north, reg);
    val &= ~(0xf << shift);
    val |= type << shift;
    pci_write_config8(north, reg, val);
}


static void sdram_enable(int controllers, const struct mem_controller *ctrl) 
{
	unsigned char i;
	static const uint8_t ramregs[] = {
		0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x56, 0x57
	};
	device_t north = 0;
	uint32_t size, base, slot, ma;
	

	/* NOP command enable */
	pci_write_config8(north,0x6b,0x01);

	/* read a double word from any addree of the dimm */
	dimm_read(0x1f000);
	udelay(200);

	/* All bank precharge Command Enable */
	pci_write_config8(north,0x6b,0x02);
	dimm_read(0x1f000);

	/* MSR Enable */
	pci_write_config8(north,0x6b,0x03);
	dimm_read(0x2000);

	dimm_read(0x800);

	/* All banks precharge Command Enable */
	pci_write_config8(north,0x6b,0x02);
	dimm_read(0x1f200);

	/* CBR Cycle Enable */
	pci_write_config8(north,0x6b,0x04);

	/* Read 8 times */
	dimm_read(0x1f300);
	udelay(100);
	dimm_read(0x1f400);
	udelay(100);
	dimm_read(0x1f500);
	udelay(100);
	dimm_read(0x1f600);
	udelay(100);
	dimm_read(0x1f700);
	udelay(100);
	dimm_read(0x1f800);
	udelay(100);
	dimm_read(0x1f900);
	udelay(100);
	dimm_read(0x1fa00);
	udelay(100);

	/* MSR Enable */
	pci_write_config8(north,0x6b,0x03);

	/* 0x150 if CAS Latency 2 or 0x350 CAS Latency 2.5 */
	dimm_read(0x350);

	/* Normal SDRAM Mode */
	pci_write_config8(north,0x6b,0x58 );


	/* Set the refresh rate */
	pci_write_config8(north,0x6a,0x43);
	pci_write_config8(north,0x67,0x22);

	/* pci */
	pci_write_config8(north,0x70,0x82);
	pci_write_config8(north,0x73,0x01);
	pci_write_config8(north,0x76,0x50);


	pci_write_config8(north,0x71,0xc8);
	

	/* graphics aperture base */
	pci_write_config8(north,0x13,0xd0);

	//pci_write_config8(north,0x56,0x10);
	//pci_write_config8(north,0x57,0x10);

	pci_write_config8(north,0xe0,0x80);
	pci_write_config8(north,0xe1,0xdf);
	pci_write_config8(north,0xe2,0x42);

	pci_write_config8(north,0xa8,0x04);
	pci_write_config8(north,0xac,0x2f);
	pci_write_config8(north,0xae,0x04);

        print_err("vt8623 done\r\n");
	dumpnorth(north);
}
