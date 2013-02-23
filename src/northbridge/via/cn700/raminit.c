/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 VIA Technologies, Inc.
 * (Written by Aaron Lwe <aaron.lwe@gmail.com> for VIA)
 * Copyright (C) 2007 Corey Osgood <corey.osgood@gmail.com>
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
#include "cn700.h"

#if CONFIG_DEBUG_RAM_SETUP
#define PRINT_DEBUG_MEM(x)		print_debug(x)
#define PRINT_DEBUG_MEM_HEX8(x)		print_debug_hex8(x)
#define PRINT_DEBUG_MEM_HEX16(x)	print_debug_hex16(x)
#define PRINT_DEBUG_MEM_HEX32(x)	print_debug_hex32(x)
#define DUMPNORTH()			dump_pci_device(PCI_DEV(0, 0, 0))
#else
#define PRINT_DEBUG_MEM(x)
#define PRINT_DEBUG_MEM_HEX8(x)
#define PRINT_DEBUG_MEM_HEX16(x)
#define PRINT_DEBUG_MEM_HEX32(x)
#define DUMPNORTH()
#endif

static void do_ram_command(device_t dev, u8 command)
{
	u8 reg;

	/* TODO: Support for multiple DIMMs. */

	reg = pci_read_config8(dev, DRAM_MISC_CTL);
	reg &= 0xf8;		/* Clear bits 2-0. */
	reg |= command;
	pci_write_config8(dev, DRAM_MISC_CTL, reg);
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
static void c7_cpu_setup(device_t dev)
{
	/* Host bus interface registers (D0F2 0x50-0x67) */
	/* Request phase control */
	pci_write_config8(dev, 0x50, 0x88);
	/* CPU Interface Control */
	pci_write_config8(dev, 0x51, 0x7a);
	pci_write_config8(dev, 0x52, 0x6f);
	/* Arbitration */
	pci_write_config8(dev, 0x53, 0x88);
	/* Miscellaneous Control */
	pci_write_config8(dev, 0x54, 0x1e);
	pci_write_config8(dev, 0x55, 0x16);
	/* Write Policy */
	pci_write_config8(dev, 0x56, 0x01);
	/* Miscellaneous Control */
	/*
	 * DRAM Operating Frequency (bits 7:5)
	 *      000 : 100MHz    001 : 133MHz
	 *      010 : 166MHz    011 : 200MHz
	 *      100 : 266MHz    101 : 333MHz
	 *      110/111 : Reserved
	 */
	/* CPU Miscellaneous Control */
	pci_write_config8(dev, 0x59, 0x44);
	/* Write Policy */
	pci_write_config8(dev, 0x5d, 0xb2);
	/* Bandwidth Timer */
	pci_write_config8(dev, 0x5e, 0x88);
	/* CPU Miscellaneous Control */
	pci_write_config8(dev, 0x5f, 0xc7);

	/* Line DRDY# Timing Control */
	pci_write_config8(dev, 0x60, 0xff);
	pci_write_config8(dev, 0x61, 0xff);
	pci_write_config8(dev, 0x62, 0x0f);
	/* QW DRDY# Timing Control */
	pci_write_config8(dev, 0x63, 0xff);
	pci_write_config8(dev, 0x64, 0xff);
	pci_write_config8(dev, 0x65, 0x0f);
	/* Read Line Burst DRDY# Timing Control */
	pci_write_config8(dev, 0x66, 0xff);
	pci_write_config8(dev, 0x67, 0x30);

	/* Host Bus I/O Circuit (see datasheet) */
	/* Host Address Pullup/down Driving */
	pci_write_config8(dev, 0x70, 0x11);
	pci_write_config8(dev, 0x71, 0x11);
	pci_write_config8(dev, 0x72, 0x11);
	pci_write_config8(dev, 0x73, 0x11);
	/* Miscellaneous Control */
	pci_write_config8(dev, 0x74, 0x35);
	/* AGTL+ I/O Circuit */
	pci_write_config8(dev, 0x75, 0x28);
	/* AGTL+ Compensation Status */
	pci_write_config8(dev, 0x76, 0x74);
	/* AGTL+ Auto Compensation Offest */
	pci_write_config8(dev, 0x77, 0x00);
	/* Host FSB CKG Control */
	pci_write_config8(dev, 0x78, 0x0a);
	/* Address/Address Clock Output Delay Control */
	pci_write_config8(dev, 0x79, 0xaa);
	/* Address Strobe Input Delay Control */
	pci_write_config8(dev, 0x7a, 0x24);
	/* Address CKG Rising/Falling Time Control */
	pci_write_config8(dev, 0x7b, 0xaa);
	/* Address CKG Clock Rising/Falling Time Control */
	pci_write_config8(dev, 0x7c, 0x00);
	/* Undefined (can't remember why I did this) */
	pci_write_config8(dev, 0x7d, 0x6d);
	pci_write_config8(dev, 0x7e, 0x00);
	pci_write_config8(dev, 0x7f, 0x00);
	pci_write_config8(dev, 0x80, 0x1b);
	pci_write_config8(dev, 0x81, 0x0a);
	pci_write_config8(dev, 0x82, 0x0a);
	pci_write_config8(dev, 0x83, 0x0a);
}

/**
 * Set up DRAM size according to SPD data. Eventually, DRAM timings should be
 * done in a similar manner.
 *
 * @param ctrl The northbridge devices and SPD addresses.
 */
static void sdram_set_size(const struct mem_controller *ctrl)
{
	u8 density, ranks, result, col;

	ranks = spd_read_byte(ctrl->channel0[0], SPD_NUM_DIMM_BANKS);
	ranks = (ranks & 0x07) + 1;
	density = spd_read_byte(ctrl->channel0[0],
				SPD_DENSITY_OF_EACH_ROW_ON_MODULE);
	switch (density) {
	case 0x80:
		result = 0x08;	/* 512MB / 64MB = 0x08 */
		break;
	case 0x40:
		result = 0x04;
		break;
	case 0x20:
		result = 0x02;
		break;
	case 0x10:
		result = 0xff;	/* 16GB */
		break;
	case 0x08:
		result = 0xff;	/* 8GB */
		break;
	case 0x04:
		result = 0xff;	/* 4GB */
		break;
	case 0x02:
		result = 0x20;	/* 2GB */
		break;
	case 0x01:
		result = 0x10;	/* 1GB */
		break;
	default:
		result = 0;
	}

	switch (result) {
	case 0xff:
		die("DRAM module size too big, not supported by CN700\n");
		break;
	case 0:
		die("DRAM module has unknown density\n");
		break;
	default:
		printk(BIOS_DEBUG, "Found %iMB of ram\n", result * ranks * 64);
	}

	pci_write_config8(ctrl->d0f3, 0x40, result);
	pci_write_config8(ctrl->d0f3, 0x48, 0x00);
	if (ranks == 2) {
		pci_write_config8(ctrl->d0f3, 0x41, result * ranks);
		pci_write_config8(ctrl->d0f3, 0x49, result);
	}
	/* Size mirror */
	pci_write_config8(ctrl->d0f7, 0xe5, (result * ranks) << 2);
	pci_write_config8(ctrl->d0f7, 0x57, (result * ranks) << 2);
	/* Low Top Address */
	pci_write_config8(ctrl->d0f3, 0x84, 0x00);
	pci_write_config8(ctrl->d0f3, 0x85, (result * ranks) << 2);
	pci_write_config8(ctrl->d0f3, 0x88, (result * ranks) << 2);

	/* Physical-Virtual Mapping */
	if (ranks == 2)
		pci_write_config8(ctrl->d0f3, 0x54,
				  1 << 7 | 0 << 4 | 1 << 3 | 1 << 0);
	if (ranks == 1)
		pci_write_config8(ctrl->d0f3, 0x54, 1 << 7 | 0 << 4);
	pci_write_config8(ctrl->d0f3, 0x55, 0x00);
	/* Virtual rank interleave, disable */
	pci_write_config32(ctrl->d0f3, 0x58, 0x00);

	/* MA Map Type */
	result = spd_read_byte(ctrl->channel0[0], SPD_NUM_BANKS_PER_SDRAM);
	if (result == 8) {
		col = spd_read_byte(ctrl->channel0[0], SPD_NUM_COLUMNS);
		switch (col) {
		case 10:
			pci_write_config8(ctrl->d0f3, 0x50, 0xa0);
			break;
		case 11:
			pci_write_config8(ctrl->d0f3, 0x50, 0xc0);
			break;
		case 12:
			pci_write_config8(ctrl->d0f3, 0x50, 0xe0);
			break;
		}
	} else if (result == 4) {
		col = spd_read_byte(ctrl->channel0[0], SPD_NUM_COLUMNS);
		switch (col) {
		case 9:
			pci_write_config8(ctrl->d0f3, 0x50, 0x00);
			break;
		case 10:
			pci_write_config8(ctrl->d0f3, 0x50, 0x20);
			break;
		case 11:
			pci_write_config8(ctrl->d0f3, 0x50, 0x40);
			break;
		case 12:
			pci_write_config8(ctrl->d0f3, 0x50, 0x60);
			break;
		}
	}
	pci_write_config8(ctrl->d0f3, 0x51, 0x00);
}

/**
 * Set up various RAM and other control registers statically. Some of these may
 * not be needed, other should be done with SPD info, but that's a project for
 * the future.
 */
static void sdram_set_registers(const struct mem_controller *ctrl)
{
	u8 reg;

	/* Set WR=5 */
	pci_write_config8(ctrl->d0f3, 0x61, 0xe0);
	/* Set CAS=4 */
	pci_write_config8(ctrl->d0f3, 0x62, 0xfa);
	/* DRAM timing-3 */
	pci_write_config8(ctrl->d0f3, 0x63, 0xca);
	/* DRAM timing-4 */
	pci_write_config8(ctrl->d0f3, 0x64, 0xcc);
	/* DIMM command / Address Selection */
	pci_write_config8(ctrl->d0f3, 0x67, 0x00);
	/* Disable cross bank/multi page mode */
	pci_write_config8(ctrl->d0f3, 0x69, 0x00);
	/* Disable refresh now */
	pci_write_config8(ctrl->d0f3, 0x6a, 0x00);

	/* Frequency 100 MHz */
	pci_write_config8(ctrl->d0f3, 0x90, 0x00);
	pci_write_config8(ctrl->d0f2, 0x57, 0x18);
	/* Allow manual DLL reset */
	pci_write_config8(ctrl->d0f3, 0x6b, 0x10);

	/* Bank/Rank Interleave Address Select */
	pci_write_config8(ctrl->d0f3, 0x52, 0x33);
	pci_write_config8(ctrl->d0f3, 0x53, 0x3f);

	/* Set to DDR2 SDRAM, BL=8 (0xc8, 0xc0 for bl=4) */
	pci_write_config8(ctrl->d0f3, 0x6c, 0xc8);

	/* DRAM Bus Turn-Around Setting */
	pci_write_config8(ctrl->d0f3, 0x60, 0x03);
	/* DRAM Arbitration Control */
	pci_write_config8(ctrl->d0f3, 0x66, 0x80);
	/*
	 * DQS Tuning: testing on a couple different boards has shown this is
	 * static, or close enough that it can be. Which is good, because the
	 * tuning function used too many registers.
	 */
	/* DQS Output Delay for Channel A */
	pci_write_config8(ctrl->d0f3, 0x70, 0x00);
	/* MD Output Delay for Channel A */
	pci_write_config8(ctrl->d0f3, 0x71, 0x01);
	pci_write_config8(ctrl->d0f3, 0x73, 0x01);

	/* DRAM arbitration timer */
	pci_write_config8(ctrl->d0f3, 0x65, 0xd9);

	/* DRAM signal timing control */
	pci_write_config8(ctrl->d0f3, 0x74, 0x01);
	pci_write_config8(ctrl->d0f3, 0x75, 0x01);
	pci_write_config8(ctrl->d0f3, 0x76, 0x06);
	pci_write_config8(ctrl->d0f3, 0x77, 0x92);
	pci_write_config8(ctrl->d0f3, 0x78, 0x83);
	pci_write_config8(ctrl->d0f3, 0x79, 0x83);
	pci_write_config8(ctrl->d0f3, 0x7a, 0x00);
	pci_write_config8(ctrl->d0f3, 0x7b, 0x10);

	/* DRAM clocking control */
	pci_write_config8(ctrl->d0f3, 0x91, 0x01);
	/* CS/CKE Clock Phase Control */
	pci_write_config8(ctrl->d0f3, 0x92, 0x02);
	/* SCMD/MA Clock Phase Control */
	pci_write_config8(ctrl->d0f3, 0x93, 0x02);
	/* DCLKO Feedback Mode Output Control */
	pci_write_config8(ctrl->d0f3, 0x94, 0x00);
	pci_write_config8(ctrl->d0f3, 0x9d, 0x0f);

	/* SDRAM ODT Control */
	pci_write_config8(ctrl->d0f3, 0xda, 0x80);
	/* Channel A DQ/DQS CKG Output Delay Control */
	pci_write_config8(ctrl->d0f3, 0xdc, 0x54);
	/* Channel A DQ/DQS CKG Output Delay Control */
	pci_write_config8(ctrl->d0f3, 0xdd, 0x55);
	/* ODT lookup table */
	pci_write_config8(ctrl->d0f3, 0xd8, 0x01);
	pci_write_config8(ctrl->d0f3, 0xd9, 0x0a);

	/* DDR SDRAM control */
	pci_write_config8(ctrl->d0f3, 0x6d, 0xc0);
	pci_write_config8(ctrl->d0f3, 0x6f, 0x41);

	/* DQ/DQS Strength Control */
	pci_write_config8(ctrl->d0f3, 0xd0, 0xaa);

	/* Compensation Control */
	pci_write_config8(ctrl->d0f3, 0xd3, 0x01); /* Enable autocompensation */
	/* ODT (some are set with driving select above) */
	pci_write_config8(ctrl->d0f3, 0xd4, 0x80);
	pci_write_config8(ctrl->d0f3, 0xd5, 0x8a);
	/* Memory Pads Driving and Range Select */
	pci_write_config8(ctrl->d0f3, 0xd6, 0xaa);

	pci_write_config8(ctrl->d0f3, 0xe0, 0xee);
	pci_write_config8(ctrl->d0f3, 0xe2, 0xac);
	pci_write_config8(ctrl->d0f3, 0xe4, 0x66);
	pci_write_config8(ctrl->d0f3, 0xe6, 0x33);
	pci_write_config8(ctrl->d0f3, 0xe8, 0x86);
	/* DQS / DQ CKG Duty Cycle Control */
	pci_write_config8(ctrl->d0f3, 0xec, 0x00);
	/* MCLK Output Duty Control */
	pci_write_config8(ctrl->d0f3, 0xee, 0x00);
	/* DQS CKG Input Delay Control */
	pci_write_config8(ctrl->d0f3, 0xef, 0x10);

	/* DRAM duty control */
	pci_write_config8(ctrl->d0f3, 0xed, 0x10);

	/* SMM and APIC decoding, we do not use SMM */
	reg = 0x29;
	pci_write_config8(ctrl->d0f3, 0x86, reg);
	/* SMM and APIC decoding mirror */
	pci_write_config8(ctrl->d0f7, 0xe6, reg);

	/* DRAM module configuration */
	pci_write_config8(ctrl->d0f3, 0x6e, 0x89);
}

static void sdram_set_post(const struct mem_controller *ctrl)
{
	device_t dev = ctrl->d0f3;

	/* Enable multipage mode. */
	pci_write_config8(dev, 0x69, 0x03);

	/* Enable refresh. */
	pci_write_config8(dev, 0x6a, 0x32);

	/* VGA device. */
	pci_write_config16(dev, 0xa0, (1 << 15));
	pci_write_config16(dev, 0xa4, 0x0010);
}

static void sdram_enable(device_t dev, unsigned long rank_address)
{
	u8 i;

	/* 1. Apply NOP. */
	PRINT_DEBUG_MEM("RAM Enable 1: Apply NOP\n");
	do_ram_command(dev, RAM_COMMAND_NOP);
	udelay(100);
	read32(rank_address + 0x10);

	/* 2. Precharge all. */
	udelay(400);
	PRINT_DEBUG_MEM("RAM Enable 2: Precharge all\n");
	do_ram_command(dev, RAM_COMMAND_PRECHARGE);
	read32(rank_address + 0x10);

	/* 3. Mode register set. */
	PRINT_DEBUG_MEM("RAM Enable 3: Mode register set\n");
	do_ram_command(dev, RAM_COMMAND_MRS);
	read32(rank_address + 0x120000);	/* EMRS DLL Enable */
	read32(rank_address + 0x800);		/* MRS DLL Reset */

	/* 4. Precharge all again. */
	PRINT_DEBUG_MEM("RAM Enable 4: Precharge all\n");
	do_ram_command(dev, RAM_COMMAND_PRECHARGE);
	read32(rank_address + 0x0);

	/* 5. Perform 8 refresh cycles. Wait tRC each time. */
	PRINT_DEBUG_MEM("RAM Enable 5: CBR\n");
	do_ram_command(dev, RAM_COMMAND_CBR);
	for (i = 0; i < 8; i++) {
		read32(rank_address + 0x20);
		udelay(100);
	}

	/* 6. Mode register set. */
	PRINT_DEBUG_MEM("RAM Enable 6: Mode register set\n");
	/* Safe value for now, BL=8, WR=5, CAS=4 */
	/*
	 * (E)MRS values are from the BPG. No direct explanation is given, but
	 * they should somehow conform to the JEDEC DDR2 SDRAM Specification
	 * (JESD79-2C).
	 */
	do_ram_command(dev, RAM_COMMAND_MRS);
	read32(rank_address + 0x002258); /* MRS command */
	read32(rank_address + 0x121c20); /* EMRS OCD Default */
	read32(rank_address + 0x120020); /* EMRS OCD Calibration Mode Exit */

	/* 8. Normal operation */
	PRINT_DEBUG_MEM("RAM Enable 7: Normal operation\n");
	do_ram_command(dev, RAM_COMMAND_NORMAL);
	read32(rank_address + 0x30);
}

/*
 * Support one DIMM with up to 2 ranks.
 */
static void ddr_ram_setup(const struct mem_controller *ctrl)
{
	u8 reg;

	c7_cpu_setup(ctrl->d0f2);
	sdram_set_registers(ctrl);
	sdram_set_size(ctrl);
	sdram_enable(ctrl->d0f3, 0);
	reg = pci_read_config8(ctrl->d0f3, 0x41);
	if (reg != 0)
		sdram_enable(ctrl->d0f3,
			     pci_read_config8(ctrl->d0f3, 0x40) << 26);
	sdram_set_post(ctrl);
}
