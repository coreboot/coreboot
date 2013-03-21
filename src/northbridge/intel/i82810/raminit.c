/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
 * Copyright (C) 2008-2009 Elia Yehuda <z4ziggy@gmail.com>
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

#include <spd.h>
#include <delay.h>
#include <stdint.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <console/console.h>
#include "i82810.h"
#include "raminit.h"

/*-----------------------------------------------------------------------------
Macros and definitions.
-----------------------------------------------------------------------------*/

/* Debugging macros. */
#if CONFIG_DEBUG_RAM_SETUP
#define PRINT_DEBUG(x...)	printk(BIOS_DEBUG, x)
#define DUMPNORTH()		dump_pci_device(PCI_DEV(0, 0, 0))
#else
#define PRINT_DEBUG(x...)
#define DUMPNORTH()
#endif

/* DRAMT[7:5] - SDRAM Mode Select (SMS). */
#define RAM_COMMAND_SELF_REFRESH 0x0 /* Disable refresh */
#define RAM_COMMAND_NORMAL	 0x1 /* Refresh: 15.6/11.7us for 100/133MHz */
#define RAM_COMMAND_NORMAL_FR	 0x2 /* Refresh: 7.8/5.85us for 100/133MHz */
#define RAM_COMMAND_NOP		 0x4 /* NOP command */
#define RAM_COMMAND_PRECHARGE	 0x5 /* All bank precharge */
#define RAM_COMMAND_MRS		 0x6 /* Mode register set */
#define RAM_COMMAND_CBR		 0x7 /* CBR */

/*
 * This table is used to translate the value read from SPD Byte 31 to a value
 * the northbridge can understand in DRP, aka Rx52[7:4], [3:0]. Where most
 * northbridges have some sort of simple calculation that can be done for this,
 * I haven't yet figured out one for this northbridge. Until someone does,
 * this table is necessary.
 */
static const u8 translate_spd_to_i82810[] = {
	/* Note: 4MB sizes are not supported, so dual-sided DIMMs with a 4MB
	 * side can't be either, at least for now.
	 */
	/* TODO: For above case, only use the other side if > 4MB, and get some
	 * of these DIMMs to test it with. Same for unsupported 128/x sizes.
	 */

		/*   SPD Byte 31	Memory Size [Side 1/2]	*/
	0xff,	/*	0x01		No memory	*/
	0xff,	/*	0x01		 4/0		*/
	0x01,	/*	0x02		 8/0		*/
	0xff,	/*	0x03		 8/4		*/
	0x04,	/*	0x04		16/0 or 16	*/
	0xff,	/*	0x05		16/4		*/
	0x05,	/*	0x06		16/8		*/
	0xff,	/*	0x07		Invalid		*/
	0x07,	/*	0x08		32/0 or 32	*/
	0xff,	/*	0x09		32/4		*/
	0xff,	/*	0x0A		32/8		*/
	0xff,	/*	0x0B		Invalid		*/
	0x08,	/*	0x0C		32/16		*/
	0xff, 0xff, 0xff, /* 0x0D-0F	Invalid		*/
	0x0a,	/*	0x10		64/0 or 64	*/
	0xff,	/*	0x11		64/4		*/
	0xff,	/*	0x12		64/8		*/
	0xff,	/*	0x13		Invalid		*/
	0xff,	/*	0x14		64/16		*/
	0xff, 0xff, 0xff, /* 0x15-17	Invalid		*/
	0x0b,	/*	0x18		64/32		*/
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 0x19-1f Invalid */
	0x0d,	/*	0x20		128/0 or 128	*/
	/* These configurations are not supported by the i810 */
	0xff,	/*	0x21		128/4		*/
	0xff,	/*	0x22		128/8		*/
	0xff,	/*	0x23		Invalid		*/
	0xff,	/*	0x24		128/16		*/
	0xff, 0xff, 0xff, /* 0x25-27	Invalid		*/
	0xff,	/*	0x28		128/32		*/
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 0x29-2f Invalid */
	0x0e,	/*	0x30		128/64		*/
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, /* 0x31-3f	Invalid		*/
	0x0f,	/*	0x40		256/0 or 256	*/
	/* Anything larger is not supported by the 82810. */
};

/*
 * Table which returns the RAM size in MB when fed the DRP[7:4] or [3:0] value.
 * Note that 2 is a value which the DRP should never be programmed to.
 * Some size values appear twice, due to single-sided vs dual-sided banks.
 */
static const u16 translate_i82810_to_mb[] = {
/* DRP	0  1 (2) 3   4   5   6   7   8   9   A   B   C    D    E    F */
/* MB */0, 8, 0, 16, 16, 24, 32, 32, 48, 64, 64, 96, 128, 128, 192, 256,
};

/* Size of bank#0 for dual-sided DIMMs */
static const u8 translate_i82810_to_bank[] = {
/* DRP	0  1 (2) 3  4  5   6   7  8   9   A  B   C   D  E    F */
/* MB */0, 0, 0, 8, 0, 16, 16, 0, 32, 32, 0, 64, 64, 0, 128, 128,
};

struct dimm_info {
	u8 ds;		/* dual-sided */
	u8 ss;		/* single-sided */
	u8 size;
};

/*-----------------------------------------------------------------------------
SDRAM configuration functions.
-----------------------------------------------------------------------------*/

static inline int spd_read_byte(unsigned device, unsigned address)
{
	return smbus_read_byte(device, address);
}

/**
 * Send the specified RAM command to all DIMMs.
 *
 * @param The RAM command to send to the DIMM(s).
 */
static void do_ram_command(u8 command)
{
	u32 addr, addr_offset;
	u16 dimm_size, dimm_start, dimm_bank;
	u8 reg8, drp;
	int i, caslatency;

	/* Configure the RAM command. */
	reg8 = pci_read_config8(PCI_DEV(0, 0, 0), DRAMT);
	reg8 &= 0x1f;		/* Clear bits 7-5. */
	reg8 |= command << 5;
	pci_write_config8(PCI_DEV(0, 0, 0), DRAMT, reg8);

	/*
	 * RAM_COMMAND_NORMAL affects only the memory controller and
	 * doesn't need to be "sent" to the DIMMs.
	 */
	if (command == RAM_COMMAND_NORMAL)
		return;

	dimm_start = 0;
	for (i = 0; i < DIMM_SOCKETS; i++) {
		/*
		 * Calculate the address offset where we need to "send" the
		 * DIMM command to. For most commands the offset is 0, only
		 * RAM_COMMAND_MRS needs special values, see below.
		 * The final address offset bits depend on three things:
		 *
		 *  (1) Some hardcoded values specified in the datasheet.
		 *  (2) Which CAS latency we will use/set. This is the SMAA[4]
		 *      bit, which is 1 for CL3, and 0 for CL2. The bitstring
		 *      so far has the form '00000001X1010', X being SMAA[4].
		 *  (3) The DIMM to which we want to send the command. For
		 *      DIMM0 no special handling is needed, but for DIMM1 we
		 *      must invert the four bits SMAA[7:4] (see datasheet).
		 *
		 * Finally, the bitstring has to be shifted 3 bits to the left.
		 * See i810 datasheet pages 43, 85, and 86 for details.
		 */
		addr_offset = 0;
		caslatency = 3; /* TODO: Dynamically get CAS latency later. */
		if (i == 0 && command == RAM_COMMAND_MRS && caslatency == 3)
			addr_offset = 0x1d0; /* DIMM0, CL3, 0000111010000 */
		if (i == 1 && command == RAM_COMMAND_MRS && caslatency == 3)
			addr_offset = 0x650; /* DIMM1, CL3, 0011001010000 */
		if (i == 0 && command == RAM_COMMAND_MRS && caslatency == 2)
			addr_offset = 0x150; /* DIMM0, CL2, 0000101010000 */
		if (i == 1 && command == RAM_COMMAND_MRS && caslatency == 2)
			addr_offset = 0x1a0; /* DIMM1, CL2, 0000110100000 */

		drp = pci_read_config8(PCI_DEV(0, 0, 0), DRP);
		drp = (drp >> (i * 4)) & 0x0f;

		dimm_size = translate_i82810_to_mb[drp];
		if (dimm_size) {
			addr = (dimm_start * 1024 * 1024) + addr_offset;
			PRINT_DEBUG("    Sending RAM command 0x%02x to 0x%08x\n", reg8, addr);
			read32(addr);
		}

		dimm_bank = translate_i82810_to_bank[drp];
		if (dimm_bank) {
			addr = ((dimm_start + dimm_bank) * 1024 * 1024) + addr_offset;
			PRINT_DEBUG("    Sending RAM command 0x%02x to 0x%08x\n", reg8, addr);
			read32(addr);
		}

		dimm_start += dimm_size;
	}
}

/*-----------------------------------------------------------------------------
DIMM-independant configuration functions.
-----------------------------------------------------------------------------*/

/*
 * Set DRP - DRAM Row Population Register (Device 0).
 */
static void spd_set_dram_size(void)
{
	/* The variables drp and dimm_size have to be ints since all the
	 * SMBus-related functions return ints, and its just easier this way.
	 */
	int i, drp, dimm_size;

	drp = 0x00;

	for (i = 0; i < DIMM_SOCKETS; i++) {
		/* First check if a DIMM is actually present. */
		if (smbus_read_byte(DIMM0 + i, 2) == 4) {
			printk(BIOS_DEBUG, "Found DIMM in slot %d\n", i);

			dimm_size = smbus_read_byte(DIMM0 + i, 31);

			printk(BIOS_DEBUG, "DIMM is %dMB\n", dimm_size * 4);

			/* The i810 can't handle DIMMs larger than 128MB per
			 * side. This will fail if the DIMM uses a
			 * non-supported DRAM tech, and can't be used until
			 * buffers are done dynamically.
			 * Note: the factory BIOS just dies if it spots this :D
			 */
			if (dimm_size > 32) {
				printk(BIOS_ERR, "DIMM row sizes larger than 128MB not"
					  "supported on i810\n");
				printk
				    (BIOS_ERR, "Attempting to treat as 128MB DIMM\n");
				dimm_size = 32;
			}

			/* This array is provided in raminit.h, because it got
			 * extremely messy. The above way is cleaner, but
			 * doesn't support any asymetrical/odd configurations.
			 */
			dimm_size = translate_spd_to_i82810[dimm_size];

			printk(BIOS_DEBUG, "After translation, dimm_size is %d\n", dimm_size);

			/* If the DIMM is dual-sided, the DRP value is +2 */
			/* TODO: Figure out asymetrical configurations. */
			if ((smbus_read_byte(DIMM0 + i, 127) | 0xf) ==
			    0xff) {
				printk(BIOS_DEBUG, "DIMM is dual-sided\n");
				dimm_size += 2;
			}
		} else {
			printk(BIOS_DEBUG, "No DIMM found in slot %d\n", i);

			/* If there's no DIMM in the slot, set value to 0. */
			dimm_size = 0x00;
		}

		/* Put in dimm_size to reflect the current DIMM. */
		drp |= dimm_size << (i * 4);
	}

	printk(BIOS_DEBUG, "DRP calculated to 0x%02x\n", drp);

	pci_write_config8(PCI_DEV(0, 0, 0), DRP, drp);
}

static void set_dram_timing(void)
{
	/* TODO, for now using default, hopefully safe values. */
	// pci_write_config8(PCI_DEV(0, 0, 0), DRAMT, 0x00);
}

/*
 * TODO: BUFF_SC needs to be set according to the DRAM tech (x8, x16,
 * or x32), but the datasheet doesn't list all the details. Currently, it
 * needs to be pulled from the output of 'lspci -xxx Rx92'.
 *
 * Common results (tested on actual hardware) are:
 *
 * (DRP: c = 128MB dual sided, d = 128MB single sided, f = 256MB dual sided)
 *
 * BUFF_SC  TOM     DRP    DIMM0                DIMM1
 * ----------------------------------------------------------------------------
 * 0x3356   128MB   0x0c   128MB dual-sided     -
 * 0xcc56   128MB   0xc0   -                    128MB dual-sided
 * 0x77da   128MB   0x0d   128MB single-sided   -
 * 0xddda   128MB   0xd0   -                    128MB single-sided
 * 0x0001   256MB   0xcc   128MB dual-sided     128MB dual-sided
 * 0x55c6   256MB   0xdd   128MB single-sided   128MB single-sided
 * 0x4445   256MB   0xcd   128MB single-sided   128MB dual-sided
 * 0x1145   256MB   0xdc   128MB dual-sided     128MB single-sided
 * 0x3356   256MB   0x0f   256MB dual-sided     -
 * 0xcc56   256MB   0xf0   -                    256MB dual-sided
 * 0x0001   384MB   0xcf   256MB dual-sided     128MB dual-sided
 * 0x0001   384MB   0xfc   128MB dual-sided     256MB dual-sided
 * 0x1145   384MB   0xdf   256MB dual-sided     128MB single-sided
 * 0x4445   384MB   0xfd   128MB single-sided   256MB dual-sided
 * 0x0001   512MB   0xff   256MB dual-sided     256MB dual-sided
 *
 * See also:
 * http://www.coreboot.org/pipermail/coreboot/2009-May/047966.html
 */
static void set_dram_buffer_strength(void)
{
	struct dimm_info d0, d1;
	u16 buff_sc;

	/* Check first slot. */
	d0.size = d0.ds = d0.ss = 0;
	if (smbus_read_byte(DIMM0, SPD_MEMORY_TYPE) == SPD_MEMORY_TYPE_SDRAM) {
		d0.size = smbus_read_byte(DIMM0, SPD_BANK_DENSITY);
		d0.ds = smbus_read_byte(DIMM0, SPD_NUM_DIMM_BANKS) > 1;
		d0.ss = !d0.ds;
	}

	/* Check second slot. */
	d1.size = d1.ds = d1.ss = 0;
	if (smbus_read_byte(DIMM0 + 1, SPD_MEMORY_TYPE)
	    == SPD_MEMORY_TYPE_SDRAM) {
		d1.size = smbus_read_byte(DIMM0 + 1, SPD_BANK_DENSITY);
		d1.ds = smbus_read_byte(DIMM0 + 1, SPD_NUM_DIMM_BANKS) > 1;
		d1.ss = !d1.ds;
	}

	buff_sc = 0;

	/* Tame the beast... */
	if ((d0.ds && d1.ds) || (d0.ds && d1.ss) || (d0.ss && d1.ds))
		buff_sc |= 1;
	if ((d0.size && !d1.size) || (!d0.size && d1.size) || (d0.ss && d1.ss))
		buff_sc |= 1 << 1;
	if ((d0.ds && !d1.size) || (!d0.size && d1.ds) || (d0.ss && d1.ss)
	   || (d0.ds && d1.ss) || (d0.ss && d1.ds))
		buff_sc |= 1 << 2;
	if ((d0.ss && !d1.size) || (!d0.size && d1.ss))
		buff_sc |= 1 << 3;
	if ((d0.size && !d1.size) || (!d0.size && d1.size))
		buff_sc |= 1 << 4;
	if ((d0.ds && !d1.size) || (!d0.size && d1.ds) || (d0.ds && d1.ss)
	   || (d0.ss && d1.ds))
		buff_sc |= 1 << 6;
	if ((d0.ss && !d1.size) || (!d0.size && d1.ss) || (d0.ss && d1.ss))
		buff_sc |= 3 << 6;
	if ((!d0.size && d1.ss) || (d0.ds && d1.ss) || (d0.ss && d1.ss))
		buff_sc |= 1 << 8;
	if (d0.size && !d1.size)
		buff_sc |= 3 << 8;
	if ((d0.ss && !d1.size) || (d0.ss && d1.ss) || (d0.ss && d1.ds))
		buff_sc |= 1 << 10;
	if (!d0.size && d1.size)
		buff_sc |= 3 << 10;
	if ((d0.size && !d1.size) || (d0.ss && !d1.size) || (!d0.size && d1.ss)
	   || (d0.ss && d1.ss) || (d0.ds && d1.ss))
		buff_sc |= 1 << 12;
	if (d0.size && !d1.size)
		buff_sc |= 1 << 13;
	if ((!d0.size && d1.size) || (d0.ss && !d1.size) || (d0.ss && d1.ss)
	   || (d0.ss && d1.ds))
		buff_sc |= 1 << 14;
	if (!d0.size && d1.size)
		buff_sc |= 1 << 15;

	printk(BIOS_DEBUG, "BUFF_SC calculated to 0x%04x\n", buff_sc);

	pci_write_config16(PCI_DEV(0, 0, 0), BUFF_SC, buff_sc);
}

/*-----------------------------------------------------------------------------
Public interface.
-----------------------------------------------------------------------------*/

void sdram_set_registers(void)
{
	u8 reg8;
	u16 did;

	did = pci_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);

	/* Ideally, this should be R/W for as many ranges as possible. */
	pci_write_config8(PCI_DEV(0, 0, 0), PAMR, 0xff);

	/* Set size for onboard-VGA framebuffer. */
	reg8 = pci_read_config8(PCI_DEV(0, 0, 0), SMRAM);
	reg8 &= 0x3f;			     /* Disable graphics (for now). */
#if CONFIG_VIDEO_MB
	if (CONFIG_VIDEO_MB == 512)
		reg8 |= (1 << 7);	     /* Enable graphics (512KB RAM). */
	else if (CONFIG_VIDEO_MB == 1)
		reg8 |= (1 << 7) | (1 << 6); /* Enable graphics (1MB RAM). */
#endif
	pci_write_config8(PCI_DEV(0, 0, 0), SMRAM, reg8);

	/* MISSC2: Bits 1, 2, 6, 7 must be set for VGA (see datasheet). */
	reg8 = pci_read_config8(PCI_DEV(0, 0, 0), MISSC2);
	reg8 |= (1 << 1); /* Instruction Parser Unit-Level Clock Gating */
	reg8 |= (1 << 2); /* Palette Load Select */
	if (did == 0x7124) {
		/* Bits 6 and 7 are only available on 82810E (not 82810). */
		reg8 |= (1 << 6); /* Text Immediate Blit */
		reg8 |= (1 << 7); /* Must be 1 as per datasheet. */
	}
	pci_write_config8(PCI_DEV(0, 0, 0), MISSC2, reg8);
}

void sdram_set_spd_registers(void)
{
	spd_set_dram_size();
	set_dram_buffer_strength();
	set_dram_timing();
}

/**
 * Enable SDRAM.
 */
void sdram_enable(void)
{
	int i;

	/* 1. Apply NOP. */
	PRINT_DEBUG("RAM Enable 1: Apply NOP\n");
	do_ram_command(RAM_COMMAND_NOP);
	udelay(200);

	/* 2. Precharge all. Wait tRP. */
	PRINT_DEBUG("RAM Enable 2: Precharge all\n");
	do_ram_command(RAM_COMMAND_PRECHARGE);
	udelay(1);

	/* 3. Perform 8 refresh cycles. Wait tRC each time. */
	PRINT_DEBUG("RAM Enable 3: CBR\n");
	for (i = 0; i < 8; i++) {
		do_ram_command(RAM_COMMAND_CBR);
		udelay(1);
	}

	/* 4. Mode register set. Wait two memory cycles. */
	PRINT_DEBUG("RAM Enable 4: Mode register set\n");
	do_ram_command(RAM_COMMAND_MRS);
	udelay(2);

	/* 5. Normal operation (enables refresh at 15.6usec). */
	PRINT_DEBUG("RAM Enable 5: Normal operation\n");
	do_ram_command(RAM_COMMAND_NORMAL);
	udelay(1);

	PRINT_DEBUG("Northbridge following SDRAM init:\n");
	DUMPNORTH();
}
