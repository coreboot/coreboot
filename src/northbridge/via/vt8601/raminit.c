#include <cpu/x86/mtrr.h>
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

#include <spd.h>

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

static void dimms_read(unsigned long x)
{
	uint8_t c;
	unsigned long eax;
	volatile unsigned long y;
	eax = x;
	for (c = 0; c < 6; c++) {
		y = *(volatile unsigned long *) eax;
		eax += 0x10000000;
	}
}

static void dimms_write(int x)
{
	uint8_t c;
	unsigned long eax = x;
	for (c = 0; c < 6; c++) {
		*(volatile unsigned long *) eax = 0;
		eax += 0x10000000;
	}
}

#if CONFIG_DEBUG_RAM_SETUP
static void dumpnorth(device_t north)
{
	unsigned int r, c;
	for (r = 0;; r += 16) {
		print_debug_hex8(r);
		print_debug(":");
		for (c = 0; c < 16; c++) {
			print_debug_hex8(pci_read_config8(north, r + c));
			print_debug(" ");
		}
		print_debug("\n");
		if (r >= 240)
			break;
	}
}
#endif

static void sdram_set_registers(const struct mem_controller *ctrl)
{
	device_t north = (device_t) PCI_DEV(0, 0, 0);

	print_err("vt8601 init starting\n");
	print_debug_hex32(north);
	print_debug(" is the north\n");
	print_debug_hex16(pci_read_config16(north, 0));
	print_debug(" ");
	print_debug_hex16(pci_read_config16(north, 2));
	print_debug("\n");

	/* All we are doing now is setting initial known-good values that will
	 * be revised later as we read SPD
	 */

	// memory clk enable. We are not using ECC
	pci_write_config8(north, 0x78, 0x01);
	print_debug_hex8(pci_read_config8(north, 0x78));

	// dram control, see the book.
#if DIMM_PC133
	pci_write_config8(north, 0x68, 0x52);
#else
	pci_write_config8(north, 0x68, 0x42);
#endif

	// dram control, see the book.
	pci_write_config8(north, 0x6B, 0x0c);

	// Initial setting, 256MB in each bank, will be rewritten later.
	pci_write_config8(north, 0x5A, 0x20);
	print_debug_hex8(pci_read_config8(north, 0x5a));
	pci_write_config8(north, 0x5B, 0x40);
	pci_write_config8(north, 0x5C, 0x60);
	pci_write_config8(north, 0x5D, 0x80);
	pci_write_config8(north, 0x5E, 0xA0);
	pci_write_config8(north, 0x5F, 0xC0);
	// It seems we have to take care of these 2 registers as if
	// they are bank 6 and 7.
	pci_write_config8(north, 0x56, 0xC0);
	pci_write_config8(north, 0x57, 0xC0);

	// SDRAM in all banks
	pci_write_config8(north, 0x60, 0x3F);

	// DRAM timing. I'm suspicious of this
	// This is for all banks, 64 is 0,1.  65 is 2,3. 66 is 4,5.
	// ras precharge 4T, RAS pulse 5T
	// cas2 is 0xd6, cas3 is 0xe6
	// we're also backing off write pulse width to 2T, so result is 0xee
#if DIMM_CL2
	pci_write_config8(north, 0x64, 0xd4);
	pci_write_config8(north, 0x65, 0xd4);
	pci_write_config8(north, 0x66, 0xd4);
#else				// CL=3
	pci_write_config8(north, 0x64, 0xe4);
	pci_write_config8(north, 0x65, 0xe4);
	pci_write_config8(north, 0x66, 0xe4);
#endif

	// dram frequency select.
	// enable 4K pages for 64M dram.
#if DIMM_PC133
	pci_write_config8(north, 0x69, 0x3c);
#else
	pci_write_config8(north, 0x69, 0xac);
#endif

	/* IMPORTANT -- disable refresh counter */
	// refresh counter, disabled.
	pci_write_config8(north, 0x6A, 0x00);

	// clkenable configuration. kevinh FIXME - add precharge
	pci_write_config8(north, 0x6C, 0x00);
	// dram read latch delay of 1 ns, MD drive 8 mA,
	// high drive strength on MA[2: 13], we#, cas#, ras#
	// As per Cindy Lee, set to 0x37, not 0x57
	pci_write_config8(north, 0x6D, 0x7f);
}

/* slot is the dram slot. Return size of side0 in lower 16-bit,
 * side1 in upper 16-bit, in units of 8MB */
static unsigned long spd_module_size(unsigned char slot)
{
	/* for all the DRAMS, see if they are there and get the size of each
	 * module. This is just a very early first cut at sizing.
	 */
	/* we may run out of registers ... */
	unsigned int banks, rows, cols;
	unsigned int value = 0;
	/* unsigned int module = ((DIMM0 + slot) << 1) + 1; */
	unsigned int module = DIMM0 + slot;

	/* is the module there? if byte 2 is not 4, then we'll assume it
	 * is useless.
	 */
	print_info("Slot ");
	print_info_hex8(slot);
	if (smbus_read_byte(module, 2) != 4) {
		print_info(" is empty\n");
		return 0;
	}
	print_info(" is SDRAM ");

	banks = smbus_read_byte(module, 17);

	/* we're going to assume symmetric banks. Sorry. */
	cols = smbus_read_byte(module, 4) & 0xf;
	rows = smbus_read_byte(module, 3) & 0xf;

	/* grand total. You have rows+cols addressing, * times of banks, times
	 * width of data in bytes */
	/* Width is assumed to be 64 bits == 8 bytes */
	value = (1 << (cols + rows));
	value *= banks * 8;
	print_info_hex32(value);
	print_info(" bytes ");
	/* Return in 8MB units */
	value >>= 23;

	/* We should have single or double side */
	if (smbus_read_byte(module, 5) == 2) {
		print_info("x2");
		value = (value << 16) | value;
	}
	print_info("\n");
	return value;
}

#if 0
static int spd_num_chips(unsigned char slot)
{
	unsigned int module = DIMM0 + slot;
	unsigned int width;

	width = smbus_read_byte(module, 13);
	if (width == 0)
		width = 8;
	return 64 / width;
}
#endif

static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
#define T133 7
	unsigned char Trp = 1, Tras = 1, casl = 2, val;
	unsigned char timing = 0xe4;
	/* read Trp */
	val = smbus_read_byte(DIMM0, 27);
	if (val < 2 * T133)
		Trp = 1;
	val = smbus_read_byte(DIMM0, 30);
	if (val < 5 * T133)
		Tras = 0;
	val = smbus_read_byte(DIMM0, 18);
	if (val < 8)
		casl = 1;
	if (val < 4)
		casl = 0;

	val = (Trp << 7) | (Tras << 6) | (casl << 4) | 4;

	print_debug_hex8(val);
	print_debug(" is the computed timing\n");
	/* don't set it. Experience shows that this screwy chipset should just
	 * be run with the most conservative timing.
	 * pci_write_config8(0, 0x64, val);
	 */
}

static void set_ma_mapping(device_t north, int slot, int type)
{
	unsigned char reg, val;
	int shift;

	reg = 0x58 + slot / 2;
	if (slot % 2 >= 1)
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
	static const uint8_t ramregs[] = {
		0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 0x56, 0x57
	};
	device_t north = 0;
	uint32_t size, base, slot, ma;
	/* begin to initialize */

	// I forget why we need this, but we do
	dimms_write(0xa55a5aa5);

	/* set NOP */
	pci_write_config8(north, 0x6C, 0x01);
	print_debug("NOP\n");
	/* wait 200us */
	// You need to do the memory reference. That causes the nop cycle.
	dimms_read(0);
	udelay(400);
	print_debug("PRECHARGE\n");
	/* set precharge */
	pci_write_config8(north, 0x6C, 0x02);
	print_debug("DUMMY READS\n");
	/* dummy reads */
	dimms_read(0);
	udelay(200);
	print_debug("CBR\n");
	/* set CBR */
	pci_write_config8(north, 0x6C, 0x04);

	/* do 8 reads and wait >100us between each - from via */
	dimms_read(0);
	udelay(200);
	dimms_read(0);
	udelay(200);
	dimms_read(0);
	udelay(200);
	dimms_read(0);
	udelay(200);
	dimms_read(0);
	udelay(200);
	dimms_read(0);
	udelay(200);
	dimms_read(0);
	udelay(200);
	dimms_read(0);
	udelay(200);
	print_debug("MRS\n");
	/* set MRS */
	pci_write_config8(north, 0x6c, 0x03);
#if DIMM_CL2
	dimms_read(0x150);
#else				// CL=3
	dimms_read(0x1d0);
#endif
	udelay(200);
	print_debug("NORMAL\n");
	/* set to normal mode */
	pci_write_config8(north, 0x6C, 0x08);

	dimms_write(0x55aa55aa);
	dimms_read(0);
	udelay(200);
	print_debug("set ref. rate\n");
	// Set the refresh rate.
#if DIMM_PC133
	pci_write_config8(north, 0x6A, 0x86);
#else
	pci_write_config8(north, 0x6A, 0x65);
#endif
	print_debug("enable multi-page open\n");
	// enable multi-page open
	pci_write_config8(north, 0x6B, 0x0d);

	base = 0;
	for (slot = 0; slot < 4; slot++) {
		size = spd_module_size(slot);
		/* side 0 */
		base += size & 0xffff;
		pci_write_config8(north, ramregs[2 * slot], base);
		/* side 1 */
		base += size >> 16;
		if (base > 0xff)
			base = 0xff;
		pci_write_config8(north, ramregs[2 * slot + 1], base);

		if (!size)
			continue;

		/* Read the row densities */
		size = smbus_read_byte(DIMM0 + slot, 0x1f);

		/* Set the MA map type.
		 *
		 * 0xa should be another option, but when
		 * it would be used is unknown.
		 */

		if (size < 16 ) /* less than 64 MB per side */
			ma = 0x0;
		else if (size < 32) /* less than 128MB per side */
			ma = 0x8;
		else if ( size < 64) /* less than 256MB per side */
			ma = 0xc;
		else /* 256MB or more per side */
			ma = 0xe;
		print_debug_hex16(ma);
		print_debug(" is the MA type\n");
		set_ma_mapping(north, slot, ma);
	}
	print_err("vt8601 done\n");
}
