/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Eric W. Biederman and Tom Zimmerman
 * Copyright (C) 2008 Arastra, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cache.h>
#include <cpu/intel/speedstep.h>
#include "raminit_ep80579.h"
#include "ep80579.h"

#define BAR 0x90000000

static void sdram_set_registers(const struct mem_controller *ctrl)
{
	static const u32 register_values[] = {
		PCI_ADDR(0, 0x00, 0, CKDIS), 0xffff0000, 0x0000ffff,
		PCI_ADDR(0, 0x00, 0, DEVPRES), 0x00000000, 0x07420001 | DEVPRES_CONFIG,
		PCI_ADDR(0, 0x00, 0, PAM-1), 0xcccccc7f, 0x33333000,
		PCI_ADDR(0, 0x00, 0, PAM+3), 0xcccccccc, 0x33333333,
		PCI_ADDR(0, 0x00, 0, DEVPRES1), 0xffffffff, 0x0040003a,
		PCI_ADDR(0, 0x00, 0, SMRBASE), 0x00000fff, BAR | 0,
	};
	int i;
	int max;

	for (i = 0; i < ARRAY_SIZE(register_values); i += 3) {
		device_t dev;
		u32 where;
		u32 reg;
		dev = (register_values[i] & ~0xff) - PCI_DEV(0, 0x00, 0) + ctrl->f0;
		where = register_values[i] & 0xff;
		reg = pci_read_config32(dev, where);
		reg &= register_values[i+1];
		reg |= register_values[i+2];
		pci_write_config32(dev, where, reg);
	}
}

struct dimm_size {
	u32 side1;
	u32 side2;
};

static struct dimm_size spd_get_dimm_size(u16 device)
{
	/* Calculate the log base 2 size of a DIMM in bits */
	struct dimm_size sz;
	int value, low, ddr2;
	sz.side1 = 0;
	sz.side2 = 0;

	/* Note it might be easier to use byte 31 here, it has the DIMM size as
	 * a multiple of 4MB.  The way we do it now we can size both
	 * sides of an assymetric dimm.
	 */
	value = spd_read_byte(device, SPD_NUM_ROWS);
	if (value < 0) goto hw_err;
	if ((value & 0xf) == 0) goto val_err;
	sz.side1 += value & 0xf;

	value = spd_read_byte(device, SPD_NUM_COLUMNS);
	if (value < 0) goto hw_err;
	if ((value & 0xf) == 0) goto val_err;
	sz.side1 += value & 0xf;

	value = spd_read_byte(device, SPD_NUM_BANKS_PER_SDRAM);
	if (value < 0) goto hw_err;
	if ((value & 0xff) == 0) goto val_err;
	sz.side1 += log2(value & 0xff);

	/* Get the module data width and convert it to a power of two */
	value = spd_read_byte(device, SPD_MODULE_DATA_WIDTH_MSB);
	if (value < 0) goto hw_err;
	value &= 0xff;
	value <<= 8;

	low = spd_read_byte(device, SPD_MODULE_DATA_WIDTH_LSB);
	if (low < 0) goto hw_err;
	value = value | (low & 0xff);
	if ((value != 72) && (value != 64)) goto val_err;
	sz.side1 += log2(value);

	/* side 2 */
	value = spd_read_byte(device, SPD_NUM_DIMM_BANKS);

	if (value < 0) goto hw_err;
	value &= 7;
	value++;
	if (value == 1) goto out;
	if (value != 2) goto val_err;

	/* Start with the symmetrical case */
	sz.side2 = sz.side1;

	value = spd_read_byte(device, SPD_NUM_ROWS);
	if (value < 0) goto hw_err;
	if ((value & 0xf0) == 0) goto out;	/* If symmetrical we are done */
	sz.side2 -= (value & 0x0f);		/* Subtract out rows on side 1 */
	sz.side2 += ((value >> 4) & 0x0f);	/* Add in rows on side 2 */

	value = spd_read_byte(device, SPD_NUM_COLUMNS);
	if (value < 0) goto hw_err;
	if ((value & 0xff) == 0) goto val_err;
	sz.side2 -= (value & 0x0f);		/* Subtract out columns on side 1 */
	sz.side2 += ((value >> 4) & 0x0f);	/* Add in columns on side 2 */
	goto out;

 val_err:
	die("Bad SPD value\n");
	/* If an hw_error occurs report that I have no memory */
 hw_err:
	sz.side1 = 0;
	sz.side2 = 0;
out:
	print_debug("dimm ");
	print_debug_hex8(device);
	print_debug(" size = ");
	print_debug_hex8(sz.side1);
	print_debug(".");
	print_debug_hex8(sz.side2);
	print_debug("\n");
	return sz;

}

static long spd_set_ram_size(const struct mem_controller *ctrl, u8 dimm_mask)
{
	int i;
	int cum;

	for (i = cum = 0; i < DIMM_SOCKETS; i++) {
		struct dimm_size sz;
		if (dimm_mask & (1 << i)) {
			sz = spd_get_dimm_size(ctrl->channel0[i]);
			if (sz.side1 < 29) {
				return -1; /* Report SPD error */
			}
			/* convert bits to multiples of 64MB */
			sz.side1 -= 29;
			cum += (1 << sz.side1);
			pci_write_config8(ctrl->f0, DRB + (i*2), cum);
			pci_write_config8(ctrl->f0, DRB+1 + (i*2), cum);
			if (spd_read_byte(ctrl->channel0[i], SPD_NUM_DIMM_BANKS) & 0x1) {
				cum <<= 1;
			}
		}
		else {
			pci_write_config8(ctrl->f0, DRB + (i*2), cum);
			pci_write_config8(ctrl->f0, DRB+1 + (i*2), cum);
		}
	}
	print_debug("DRB = ");
	print_debug_hex32(pci_read_config32(ctrl->f0, DRB));
	print_debug("\n");

	cum >>= 1;
	/* set TOM top of memory */
	pci_write_config16(ctrl->f0, TOM, cum);
	print_debug("TOM = ");
	print_debug_hex16(cum);
	print_debug("\n");
	/* set TOLM top of low memory */
	if (cum > 0x18) {
		cum = 0x18;
	}
	cum <<= 11;
	pci_write_config16(ctrl->f0, TOLM, cum);
	print_debug("TOLM = ");
	print_debug_hex16(cum);
	print_debug("\n");
	return 0;
}


static u8 spd_detect_dimms(const struct mem_controller *ctrl)
{
	u8 dimm_mask = 0;
	int i;
	for (i = 0; i < DIMM_SOCKETS; i++) {
		int byte;
		u16 device;
		device = ctrl->channel0[i];
		if (device) {
			byte = spd_read_byte(device, SPD_MEMORY_TYPE);
			print_debug("spd ");
			print_debug_hex8(device);
			print_debug(" = ");
			print_debug_hex8(byte);
			print_debug("\n");
			if (byte == 8) {
				dimm_mask |= (1 << i);
			}
		}
	}
	return dimm_mask;
}


static int spd_set_row_attributes(const struct mem_controller *ctrl,
				  u8 dimm_mask)
{
	int value;
	int i;

	for (i = 0; i < DIMM_SOCKETS; i++) {
		u32 dra = 0;
		int reg = 0;

		if (!(dimm_mask & (1 << i))) {
			continue;
		}

		value = spd_read_byte(ctrl->channel0[i], SPD_NUM_ROWS);
		if (value < 0) die("Bad SPD data\n");
		if ((value & 0xf) == 0) die("Invalid # of rows\n");
		dra |= (((value-13) & 0x7) << 23);
		dra |= (((value-13) & 0x7) << 29);
		reg += value & 0xf;

		value = spd_read_byte(ctrl->channel0[i], SPD_NUM_COLUMNS);
		if (value < 0) die("Bad SPD data\n");
		if ((value & 0xf) == 0) die("Invalid # of columns\n");
		dra |= (((value-10) & 0x7) << 20);
		dra |= (((value-10) & 0x7) << 26);
		reg += value & 0xf;

		value = spd_read_byte(ctrl->channel0[i], SPD_NUM_BANKS_PER_SDRAM);
		if (value < 0) die("Bad SPD data\n");
		if ((value & 0xff) == 0) die("Invalid # of banks\n");
		reg += log2(value & 0xff);

		print_debug("dimm ");
		print_debug_hex8(i);
		print_debug(" reg = ");
		print_debug_hex8(reg);
		print_debug("\n");

		/* set device density */
		dra |= ((31-reg));
		dra |= ((31-reg) << 6);

		/* set device width (x8) */
		dra |= (1 << 4);
		dra |= (1 << 10);

		/* set device type (registered) */
		dra |= (1 << 14);

		/* set number of ranks (0=single, 1=dual) */
		value = spd_read_byte(ctrl->channel0[i], SPD_NUM_DIMM_BANKS);
		dra |= ((value & 0x1) << 17);

		print_debug("DRA");
		print_debug_hex8(i);
		print_debug(" = ");
		print_debug_hex32(dra);
		print_debug("\n");

		pci_write_config32(ctrl->f0, DRA + (i*4), dra);
	}
	return 0;
}


static u32 spd_set_drt_attributes(const struct mem_controller *ctrl,
		u8 dimm_mask, u32 drc)
{
	int i;
	u32 val, val1;
	u32 cl;
	u32 trc = 0;
	u32 trfc = 0;
	u32 tras = 0;
	u32 trtp = 0;
	u32 twtr = 0;
	int index = drc & 0x00000003;
	int ci;
	static const u8 latencies[] = { /* 533, 800, 400, 667 */
		0x10, 0x60, 0x10, 0x20 };
	static const u32 drt0[] = { /* 533, 800, 400, 667 */
		0x24240002, 0x24360002, 0x24220002, 0x24360002 };
	static const u32 drt1[] = { /* 533, 800, 400, 667 */
		0x00400000, 0x00900000, 0x00200000, 0x00700000 };
	static const u32 magic[] = { /* 533, 800, 400, 667 */
		0x007b8221, 0x00b94331, 0x005ca1a1, 0x009a62b1 };
	static const u32 mrs[] = { /* 533, 800, 400, 667 */
		0x07020000, 0x0b020000, 0x05020000, 0x09020000 };
	static const int cycle[] = { /* 533, 800, 400, 667 */
		15, 10, 20, 12 }; /* cycle time in 1/4 ns units */
	static const int byte40rem[] = {
		0, 1, 2, 2, 3, 3, 0, 0 }; /* byte 40 remainder in 1/4 ns units */

	/* CAS latency in cycles */
	val = latencies[index];
	for (i = 0; i < DIMM_SOCKETS; i++) {
		if (!(dimm_mask & (1 << i)))
			continue;
		val &= spd_read_byte(ctrl->channel0[i], SPD_ACCEPTABLE_CAS_LATENCIES);
	}
	if (val & 0x10)
		cl = 4;
	else if (val & 0x20)
		cl = 5;
	else if (val & 0x40)
		cl = 6;
	else
		die("CAS latency mismatch\n");
	print_debug("cl = ");
	print_debug_hex8(cl);
	print_debug("\n");

	ci = cycle[index];

	/* Trc, Trfc in cycles */
	for (i = 0; i < DIMM_SOCKETS; i++) {
		if (!(dimm_mask & (1 << i)))
			continue;
		val1 = spd_read_byte(ctrl->channel0[i], SPD_BYTE_41_42_EXTENSION);
		val = spd_read_byte(ctrl->channel0[i], SPD_MIN_ACT_TO_ACT_AUTO_REFRESH);
		val <<= 2; /* convert to 1/4 ns */
		val += byte40rem[(val1 >> 4) & 0x7];
		val = (val + ci - 1) / ci + 1; /* convert to cycles */
		if (trc < val)
			trc = val;
		val = spd_read_byte(ctrl->channel0[i], SPD_MIN_AUTO_REFRESH_TO_ACT);
		val <<= 2; /* convert to 1/4 ns */
		if (val1 & 0x01)
			val += 1024;
		val += byte40rem[(val1 >> 1) & 0x7];
		val = (val + ci - 1) / ci; /* convert to cycles */
		if (trfc < val)
			trfc = val;
	}
	print_debug("trc = ");
	print_debug_hex8(trc);
	print_debug("\n");
	print_debug("trfc = ");
	print_debug_hex8(trfc);
	print_debug("\n");

	/* Tras, Trtp, Twtr in cycles */
	for (i = 0; i < DIMM_SOCKETS; i++) {
		if (!(dimm_mask & (1 << i)))
			continue;
		val = spd_read_byte(ctrl->channel0[i], SPD_MIN_ACTIVE_TO_PRECHARGE_DELAY);
		val <<= 2; /* convert to 1/4 ns */
		val = (val + ci - 1) / ci; /* convert to cycles */
		if (tras < val)
			tras = val;
		val = spd_read_byte(ctrl->channel0[i], SPD_INT_READ_TO_PRECHARGE_DELAY);
		val = (val + ci - 1) / ci; /* convert to cycles */
		if (trtp < val)
			trtp = val;
		val = spd_read_byte(ctrl->channel0[i], SPD_INT_WRITE_TO_READ_DELAY);
		val = (val + ci - 1) / ci; /* convert to cycles */
		if (twtr < val)
			twtr = val;
	}
	print_debug("tras = ");
	print_debug_hex8(tras);
	print_debug("\n");
	print_debug("trtp = ");
	print_debug_hex8(trtp);
	print_debug("\n");
	print_debug("twtr = ");
	print_debug_hex8(twtr);
	print_debug("\n");

	val = (drt0[index] | ((trc - 11) << 12) | ((cl - 3) << 9)
	       | ((cl - 3) << 6) | ((cl - 3) << 3));
	print_debug("drt0 = ");
	print_debug_hex32(val);
	print_debug("\n");
	pci_write_config32(ctrl->f0, DRT0, val);

	val = (drt1[index] | ((tras - 8) << 28) | ((trtp - 2) << 25)
	       | (twtr << 15));
	print_debug("drt1 = ");
	print_debug_hex32(val);
	print_debug("\n");
	pci_write_config32(ctrl->f0, DRT1, val);

	val = (magic[index]);
	print_debug("magic = ");
	print_debug_hex32(val);
	print_debug("\n");
	pci_write_config32(PCI_DEV(0, 0x08, 0), 0xcc, val);

	val = (mrs[index] | (cl << 20));
	print_debug("mrs = ");
	print_debug_hex32(val);
	print_debug("\n");
	return val;
}

static int spd_set_dram_controller_mode(const struct mem_controller *ctrl,
					u8 dimm_mask)
{
	int value;
	int drc = 0;
	int i;
	msr_t msr;
	u8 cycle = 0x25;

	for (i = 0; i < DIMM_SOCKETS; i++) {
		if (!(dimm_mask & (1 << i)))
			continue;
		if ((spd_read_byte(ctrl->channel0[i], SPD_MODULE_DATA_WIDTH_LSB) & 0xf0) != 0x40)
			die("ERROR: Only 64-bit DIMMs supported\n");
		if (!(spd_read_byte(ctrl->channel0[i], SPD_DIMM_CONFIG_TYPE) & 0x02))
			die("ERROR: Only ECC DIMMs supported\n");
		if (spd_read_byte(ctrl->channel0[i], SPD_PRIMARY_SDRAM_WIDTH) != 0x08)
			die("ERROR: Only x8 DIMMs supported\n");

		value = spd_read_byte(ctrl->channel0[i], SPD_MIN_CYCLE_TIME_AT_CAS_MAX);
		if (value > cycle)
			cycle = value;
	}
	print_debug("cycle = ");
	print_debug_hex8(cycle);
	print_debug("\n");

	drc |= (1 << 20); /* enable ECC */
	drc |= (3 << 30); /* enable CKE on each DIMM */
	drc |= (1 << 4); /* enable CKE globally */

	/* TODO check: */
	/* set front side bus speed */
	msr = rdmsr(MSR_FSB_FREQ); /* returns 0 on Pentium M 90nm */
	print_debug("MSR FSB_FREQ(0xcd) = ");
	print_debug_hex32(msr.hi);
	print_debug_hex32(msr.lo);
	print_debug("\n");

	/* TODO check that this msr really indicates fsb speed! */
	if (msr.lo & 0x07) {
		print_info("533 MHz FSB\n");
		if (cycle <= 0x25) {
			drc |= 0x5;
			print_info("400 MHz DDR\n");
		} else if (cycle <= 0x30) {
			drc |= 0x7;
			print_info("333 MHz DDR\n");
		} else if (cycle <= 0x3d) {
			drc |= 0x4;
			print_info("266 MHz DDR\n");
		} else {
			drc |= 0x2;
			print_info("200 MHz DDR\n");
		}
	}
	else {
		print_info("400 MHz FSB\n");
		if (cycle <= 0x30) {
			drc |= 0x7;
			print_info("333 MHz DDR\n");
		} else if (cycle <= 0x3d) {
			drc |= 0x0;
			print_info("266 MHz DDR\n");
		} else {
			drc |= 0x2;
			print_info("200 MHz DDR\n");
		}
	}

	print_debug("DRC = ");
	print_debug_hex32(drc);
	print_debug("\n");

	return drc;
}

static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
	u8 dimm_mask;
	int i;

	/* Test if we can read the SPD */
	dimm_mask = spd_detect_dimms(ctrl);
	if (!(dimm_mask & ((1 << DIMM_SOCKETS) - 1))) {
		print_err("No memory for this cpu\n");
		return;
	}
	return;
}

static void set_on_dimm_termination_enable(const struct mem_controller *ctrl)
{
	u8 c1,c2;
        u32 dimm,i;
        u32 data32;
	u32 t4;

	/* Set up northbridge values */
	/* ODT enable */
  	pci_write_config32(ctrl->f0, SDRC, 0xa0002c30);

	c1 = pci_read_config8(ctrl->f0, DRB);
	c2 = pci_read_config8(ctrl->f0, DRB+2);
	if (c1 == c2) {
		/* 1 single-rank DIMM */
		data32 = 0x00000010;
	}
	else {
		/* 2 single-rank DIMMs or 1 double-rank DIMM */
		data32 = 0x00002010;
	}

	print_debug("ODT Value = ");
	print_debug_hex32(data32);
	print_debug("\n");

  	pci_write_config32(ctrl->f0, DDR2ODTC, data32);

	for (i = 0; i < 2; i++) {
		print_debug("ODT CS");
		print_debug_hex8(i);
		print_debug("\n");

		write32(BAR+DCALADDR, 0x0b840001);
		write32(BAR+DCALCSR, 0x80000003 | ((i+1)<<21));
		do data32 = read32(BAR+DCALCSR);
		while (data32 & 0x80000000);
	}
}


static void dump_dcal_regs(void)
{
	int i;
	for (i = 0x0; i < 0x2a0; i += 4) {
		if ((i % 16) == 0) {
			print_debug("\n");
			print_debug_hex16(i);
			print_debug(": ");
		}
		print_debug_hex32(read32(BAR+i));
		print_debug(" ");
	}
	print_debug("\n");
}


static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;
	int cs;
	long mask;
	u32 drc;
	u32 data32;
	u32 mode_reg;
	msr_t msr;
	u16 data16;

	mask = spd_detect_dimms(ctrl);
	print_debug("Starting SDRAM Enable\n");

	/* Set DRAM type and Front Side Bus frequency */
	drc = spd_set_dram_controller_mode(ctrl, mask);
	if (drc == 0) {
		die("Error calculating DRC\n");
	}
	data32 = drc & ~(3 << 20);  /* clear ECC mode */
	data32 = data32 | (3 << 5);  /* temp turn off ODT */
  	/* Set DRAM controller mode */
  	pci_write_config32(ctrl->f0, DRC, data32);

	/* Turn the clocks on */
  	pci_write_config16(ctrl->f0, CKDIS, 0x0000);

	/* Program row size */
	spd_set_ram_size(ctrl, mask);

	/* Program row attributes */
	spd_set_row_attributes(ctrl, mask);

	/* Program timing values */
	mode_reg = spd_set_drt_attributes(ctrl, mask, drc);

	dump_dcal_regs();

	/* Apply NOP */
	for (cs = 0; cs < 2; cs++) {
		print_debug("NOP CS");
		print_debug_hex8(cs);
		print_debug("\n");
		udelay(16);
		write32(BAR+DCALCSR, (0x00000000 | ((cs+1)<<21)));
		write32(BAR+DCALCSR, (0x80000000 | ((cs+1)<<21)));
		do data32 = read32(BAR+DCALCSR);
		while (data32 & 0x80000000);
	}

	/* Apply NOP */
	udelay(16);
	for (cs = 0; cs < 2; cs++) {
		print_debug("NOP CS");
		print_debug_hex8(cs);
		print_debug("\n");
		write32(BAR + DCALCSR, (0x80000000 | ((cs+1)<<21)));
		do data32 = read32(BAR+DCALCSR);
		while (data32 & 0x80000000);
	}

	/* Precharge all banks */
	udelay(16);
	for (cs = 0; cs < 2; cs++) {
		print_debug("Precharge CS");
		print_debug_hex8(cs);
		print_debug("\n");
		write32(BAR+DCALADDR, 0x04000000);
		write32(BAR+DCALCSR, (0x80000002 | ((cs+1)<<21)));
		do data32 = read32(BAR+DCALCSR);
		while (data32 & 0x80000000);
	}

	/* EMRS: Enable DLLs, set OCD calibration mode to default */
	udelay(16);
	for (cs = 0; cs < 2; cs++) {
		print_debug("EMRS CS");
		print_debug_hex8(cs);
		print_debug("\n");
		write32(BAR+DCALADDR, 0x0b840001);
		write32(BAR+DCALCSR, (0x80000003 | ((cs+1)<<21)));
		do data32 = read32(BAR+DCALCSR);
		while (data32 & 0x80000000);
	}
	/* MRS: Reset DLLs */
	udelay(16);
	for (cs = 0; cs < 2; cs++) {
		print_debug("MRS CS");
		print_debug_hex8(cs);
		print_debug("\n");
		write32(BAR+DCALADDR, mode_reg);
		write32(BAR+DCALCSR, (0x80000003 | ((cs+1)<<21)));
		do data32 = read32(BAR+DCALCSR);
		while (data32 & 0x80000000);
	}

	/* Precharge all banks */
	udelay(48);
	for (cs = 0; cs < 2; cs++) {
		print_debug("Precharge CS");
		print_debug_hex8(cs);
		print_debug("\n");
		write32(BAR+DCALADDR, 0x04000000);
		write32(BAR+DCALCSR, (0x80000002 | ((cs+1)<<21)));
		do data32 = read32(BAR+DCALCSR);
		while (data32 & 0x80000000);
	}

	/* Do 2 refreshes */
	for (i = 0; i < 2; i++) {
		udelay(16);
		for (cs = 0; cs < 2; cs++) {
			print_debug("Refresh CS");
			print_debug_hex8(cs);
			print_debug("\n");
			write32(BAR+DCALCSR, (0x80000001 | ((cs+1)<<21)));
			do data32 = read32(BAR+DCALCSR);
			while (data32 & 0x80000000);
		}
	}

	/* MRS: Set DLLs to normal */
	udelay(16);
	for (cs = 0; cs < 2; cs++) {
		print_debug("MRS CS");
		print_debug_hex8(cs);
		print_debug("\n");
		write32(BAR+DCALADDR, (mode_reg & ~(1<<24)));
		write32(BAR+DCALCSR, (0x80000003 | ((cs+1)<<21)));
		do data32 = read32(BAR+DCALCSR);
		while (data32 & 0x80000000);
	}

	/* EMRS: Enable DLLs */
	udelay(16);
	for (cs = 0; cs < 2; cs++) {
		print_debug("EMRS CS");
		print_debug_hex8(cs);
		print_debug("\n");
		write32(BAR+DCALADDR, 0x0b840001);
		write32(BAR+DCALCSR, (0x80000003 | ((cs+1)<<21)));
		do data32 = read32(BAR+DCALCSR);
		while (data32 & 0x80000000);
        }

	udelay(16);
	/* No command */
	write32(BAR+DCALCSR, 0x0000000f);

	write32(BAR, 0x00100000);

	/* Enable on-DIMM termination */
	set_on_dimm_termination_enable(ctrl);

	dump_dcal_regs();

	/* Receive enable calibration */
	udelay(16);
	for (cs = 0; cs < 1; cs++) {
		print_debug("receive enable calibration CS");
		print_debug_hex8(cs);
		print_debug("\n");
		write32(BAR+DCALCSR, (0x8000000c | ((cs+1)<<21)));
		do data32 = read32(BAR+DCALCSR);
		while (data32 & 0x80000000);
	}

	dump_dcal_regs();

	/* Adjust RCOMP */
	data32 = read32(BAR+DDRIOMC2);
	data32 &= ~(0xf << 16);
	data32 |= (0xb << 16);
	write32(BAR+DDRIOMC2, data32);

	dump_dcal_regs();

	data32 = drc & ~(3 << 20);  /* clear ECC mode */
	pci_write_config32(ctrl->f0, DRC, data32);
	write32(BAR+DCALCSR, 0x0008000f);

	/* Clear memory and init ECC */
	for (cs = 0; cs < 2; cs++) {
		if (!(mask & (1<<cs)))
			continue;
		print_debug("clear memory CS");
		print_debug_hex8(cs);
		print_debug("\n");
		write32(BAR+MBCSR, 0xa00000f0 | ((cs+1)<<20) | (0<<16));
		do data32 = read32(BAR+MBCSR);
		while (data32 & 0x80000000);
		if (data32 & 0x40000000)
			print_debug("failed!\n");
	}

	/* Clear read/write FIFO pointers */
	print_debug("clear read/write fifo pointers\n");
	write32(BAR+DDRIOMC2, read32(BAR+DDRIOMC2) | (1<<15));
	udelay(16);
	write32(BAR+DDRIOMC2, read32(BAR+DDRIOMC2) & ~(1<<15));
	udelay(16);

	dump_dcal_regs();

	print_debug("Done\n");

	/* Set initialization complete */
	drc |= (1 << 29);
	drc |= (3 << 30);
	data32 = drc & ~(3 << 20);  /* clear ECC mode */
	pci_write_config32(ctrl->f0, DRC, data32);

	/* Set the ECC mode */
	pci_write_config32(ctrl->f0, DRC, drc);

	/* The memory is now set up--use it */
	cache_ramstage();
}

static inline int memory_initialized(void)
{
	return pci_read_config32(PCI_DEV(0, 0x00, 0), DRC) & (1 << 29);
}
