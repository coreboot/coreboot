/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Corey Osgood <corey.osgood@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <io.h>
#include <config.h>
#include <spd.h>
#include "cn700.h"

/* NOTE: We can't use floats in CAR, even though gcc will allow it. "The 
 * problem is that many x86 CPUs have cache bypasses for x87/MMX/SSE/whatever
 * instructions and that would cause them not to be filled from CAR. At least 
 * that's my understanding from reading a few too many of these databooks."
 * -Carl-Daniel Hailfinger
 */

/* TODO: This only supports DDR2 memory, but CN700 also supports DDR.
 * Need a detection routine for dual-sided dimms in do_ram_size().
 * Some of this should be split off into a stage1.c, and done there.
 * Anything that doesn't directly affect ram init should be moved to a seperate
 *	function, something like cn700_init(), including VGA and PCI bridge init
 * Ram speed is currently stuck at 100MHz, because any time it's changed the
 * 	system locks up. This NEEDS to be fixed. A couple other functions need
 * 	to be fixed to support that change, including DRAM refresh enable.
 */

static void do_ram_command(struct board_info *dev, u8 command, u32 addr_offset)
{
	u8 reg8;
	int i, j;

	reg8 = pci_conf1_read_config8(dev->d0f3, DRAM_MISC_CTL);
	reg8 &= 0xf8;		/* Clear bits 2-0. */
	reg8 |= command;
	pci_conf1_write_config8(dev->d0f3, DRAM_MISC_CTL, reg8);

	printk(BIOS_SPEW, "   Sending RAM command ");
	switch(command) {
		case RAM_COMMAND_NORMAL:
			printk(BIOS_SPEW, "'Normal' to 0x%x", addr_offset);
			break;

		case RAM_COMMAND_NOP:
			printk(BIOS_SPEW, "'No-op' to 0x%x", addr_offset);
			break;

		case RAM_COMMAND_PRECHARGE:
			printk(BIOS_SPEW, "'Precharge' to 0x%x", addr_offset);
			break;

		case RAM_COMMAND_MRS:
			printk(BIOS_SPEW, "'MRS' to 0x%x", addr_offset);
			/* EMRS DLL Enable */
			if (addr_offset == 0x12000)
			{
				/* MRS DLL Reset */
				readl((void *)0x800);
			} 
			else /* MRS Command */
			{
				/* EMRS OCD Default */
				readl((void *)0x121c20);
				/* EMRS Exit OCD Calibration */
				readl((void *)0x120020);
			}
			break;

		case RAM_COMMAND_CBR:
			for(j = 0; j < 8; j++) {
				readl((void *)((reg8 << 26) + addr_offset));
				udelay(10);
			}
			printk(BIOS_SPEW, "'CBR' to 0x%x", addr_offset);
			break;
	};

	/* NOTE: Dual-sided and multi-dimm ready */
	readl((void *)addr_offset);
	for(i = 0; i < (ARRAY_SIZE(dev->spd_channel0) * 2); i++)
	{
		reg8 = pci_conf1_read_config8(dev->d0f3, RANK0_START + i);
		if (reg8)
		{
			readl((void *)((reg8 << 26) + addr_offset));
			printk(BIOS_SPEW, ", 0x%x", (reg8 << 26) + addr_offset);
			if (command == RAM_COMMAND_MRS)
			{
				if (addr_offset == 0x12000)
				{
					readl((void *)((reg8 << 26) + 0x800));
				}
				else
				{
					readl((void *)((reg8 << 26) + 0x121c20));
					readl((void *)((reg8 << 26) + 0x120020));
				}
			}
			else if (command == RAM_COMMAND_CBR)
			{
				for(j = 0; j < 8; j++) {
					readl((void *)((reg8 << 26) + addr_offset));
					udelay(100);
				}
			}
		}
	}
	printk(BIOS_SPEW, "\n");
}

/**
 * Set up various ram and other control registers statically. Some of these may 
 * not be needed, other should be done with spd info, but that's a project for
 * the future
 */
void sdram_set_registers(struct board_info *dev)
{
	/* DQ/DQS Strength Control */
	pci_conf1_write_config8(dev->d0f3, 0xd0, 0x88);
	pci_conf1_write_config8(dev->d0f3, 0xd1, 0x8b);
	pci_conf1_write_config8(dev->d0f3, 0xd2, 0x89);
	/* SMM and APIC Decoding */
	pci_conf1_write_config8(dev->d0f3, 0x86, (1 << 4));
	pci_conf1_write_config8(dev->d0f7, 0xe6, (1 << 4));
	
	/* Driving selection */
	/* DQ / DQS ODT Driving and Range Select */
	pci_conf1_write_config8(dev->d0f3, 0xd5, 0x8a);
	/* Memory Pads Driving and Range Select */
	pci_conf1_write_config8(dev->d0f3, 0xd6, 0xaa);
	/* DRAM Driving – Group DQS */
	pci_conf1_write_config8(dev->d0f3, 0xe0, 0xee);
	/* DRAM Driving – Group DQ (DQ, MPD, DQM) */
	pci_conf1_write_config8(dev->d0f3, 0xe2, 0xac);//ba
	/* DRAM Driving – Group CS */
	pci_conf1_write_config8(dev->d0f3, 0xe4, 0x66);
	/* DRAM Driving – Group MA */
	pci_conf1_write_config8(dev->d0f3, 0xe8, 0x86);
	/* DRAM Driving – Group MCLK */
	pci_conf1_write_config8(dev->d0f3, 0xe6, 0xaa);

	/* ODT (some are set with driving select above) */
	/* Memory Pad ODT Pullup / Pulldown Control */
	pci_conf1_write_config8(dev->d0f3, 0xd4, 0x0a);
	/* Memory Ranks ODT Lookup Table */
	pci_conf1_write_config8(dev->d0f3, 0xd8, 0x00);//was 1
	/* Compensation Control */
	pci_conf1_write_config8(dev->d0f3, 0xd3, 0x89);//enable auto compensation
	
	/* MCLKO Phase Control */
	pci_conf1_write_config8(dev->d0f3, 0x91, 0x02);
	/* CS/CKE Clock Phase Control */
	pci_conf1_write_config8(dev->d0f3, 0x92, 0x06);
	/* SCMD/MA Clock Phase Control */
	pci_conf1_write_config8(dev->d0f3, 0x93, 0x07);
	/* Channel A DQS Input Capture Range Control */
	pci_conf1_write_config8(dev->d0f3, 0x78, 0x83);
	/* DQS Input Capture Range Control */
	/* Set in accordance with the BIOS update note */
	pci_conf1_write_config8(dev->d0f3, 0x7a, 0x00);
	/* DQS Input Delay Offset Control */
	pci_conf1_write_config8(dev->d0f3, 0x7c, 0x00);
	/* SDRAM ODT Control */
	pci_conf1_write_config8(dev->d0f3, 0xda, 0x80);
	/* DQ/DQS CKG Output Delay Control - I */
	pci_conf1_write_config8(dev->d0f3, 0xdc, 0xff);
	/* DQ/DQS CKG Output Delay Control - II */
	pci_conf1_write_config8(dev->d0f3, 0xdd, 0xff);
	/* DQS / DQ CKG Duty Cycle Control */
	pci_conf1_write_config8(dev->d0f3, 0xec, 0x88);
	/* MCLK Output Duty Control */
	pci_conf1_write_config8(dev->d0f3, 0xee, 0x00);
	pci_conf1_write_config8(dev->d0f3, 0xed, 0x10);
	/* DQS CKG Input Delay Control */
	pci_conf1_write_config8(dev->d0f3, 0xef, 0x10);

	pci_conf1_write_config8(dev->d0f3, 0x77, 0x9d);
	pci_conf1_write_config8(dev->d0f3, 0x79, 0x83);
	pci_conf1_write_config16(dev->d0f3, 0x88, 0x0020);
	
	/* VLink Control */
	pci_conf1_write_config8(dev->d0f7, 0xb0, 0x05);
	pci_conf1_write_config8(dev->d0f7, 0xb1, 0x01);
	
	/* Memory base */
	pci_conf1_write_config16(dev->d1f0, 0x20, 0xfb00);
	/* Memory limit */
	pci_conf1_write_config16(dev->d1f0, 0x22, 0xfcf0);
	/* Prefetch memory base */
	pci_conf1_write_config16(dev->d1f0, 0x24, 0xf400);
	/* Prefetch memory limit */
	pci_conf1_write_config16(dev->d1f0, 0x26, 0xf7f0);
	/* PCI to PCI bridge control */
	pci_conf1_write_config16(dev->d1f0, 0x3e, 0x0008);
	
	/* CPU to PCI flow control 1 */
	pci_conf1_write_config8(dev->d1f0, 0x40, 0x83);
	pci_conf1_write_config8(dev->d1f0, 0x41, 0xc3);//clear reset error, set to 43
	pci_conf1_write_config8(dev->d1f0, 0x42, 0xe2);
	pci_conf1_write_config8(dev->d1f0, 0x43, 0x44);
	pci_conf1_write_config8(dev->d1f0, 0x44, 0x34);
	pci_conf1_write_config8(dev->d1f0, 0x45, 0x72);

	/* Disable cross bank/multi page mode */
	pci_conf1_write_config8(dev->d0f3, DDR_PAGE_CTL, 0x80);
	pci_conf1_write_config8(dev->d0f3, DRAM_REFRESH_COUNTER, 0x00);

	/* These registers set up some very conservative ram timings for
	 * testing purposes. If you're having trouble with your ram not
	 * working, try uncommenting them. You don't have to comment anything
	 * out, the code will think there's ram that requires these values,
	 * and use them.
	 * TODO: #if and option in Kconfig
	 */
	/* Set WR=5 and RFC */
	//pci_conf1_write_config8(dev->d0f3, 0x61, 0x94);
	/* Set CAS=5 */
	//pci_conf1_write_config8(dev->d0f3, 0x62, 0x7a);
	//pci_conf1_write_config8(dev->d0f3, 0x63, 0x00);
	//pci_conf1_write_config8(dev->d0f3, 0x64, 0x88);

	/* Set to DDR2 sdram, BL=8 (0xc8, 0xc0 for BL=4) */
	pci_conf1_write_config8(dev->d0f3, 0x6c, 0xc8);
	/* Allow manual dll reset */
	pci_conf1_write_config8(dev->d0f3, 0x6b, 0x10);
	
	pci_conf1_write_config8(dev->d0f3, 0x6e, 0x89);
	pci_conf1_write_config8(dev->d0f3, 0x67, 0x50);
	pci_conf1_write_config8(dev->d0f3, 0x65, 0xd9);
	
	/* Zero the MA map field */
	pci_conf1_write_config16(dev->d0f3, 0x50, 0x0);

	/* BA0-2 Selection. Don't mess with */
	pci_conf1_write_config8(dev->d0f3, 0x52, 0x33);
	pci_conf1_write_config8(dev->d0f3, 0x53, 0x3f);
	
	/* Disable bank interleaving. */
	pci_conf1_write_config32(dev->d0f3, 0x58, 0x00000000);
	pci_conf1_write_config8(dev->d0f3, 0x88, 0x08);

	/* Some DQS control stuffs */
	pci_conf1_write_config8(dev->d0f3, 0x74, 0x04);
	pci_conf1_write_config8(dev->d0f3, 0x75, 0x04);
	pci_conf1_write_config8(dev->d0f3, 0x76, 0x00);
}

/**
 * Check the detected timing against timings supported by the CN700, and
 * adjust as necessary.
 *
 * @param val The value to be checked.
 * @param min Minimum value supported by the CN700.
 * @param max Maximum value supported by the CN700.
 * @return ret The checked and/or modified value.
 */
static u16 check_timing(u16 val, u16 min, u16 max)
{
	int ret;
	if (val < min)
		ret = min;
	else if (val > max)
		ret = max;
	else
		ret = val;
	return ret;
}

/**
 * Set the DRAM size registers
 *
 * @param dev: struct containing spd addresses and pci device locations
 * @param i: Which DRAM Rank to be sizing
 */ 
static int do_ram_size(struct board_info *dev, int i)
{
	u8 spd_data, j, reg8 = 0;

	/* DRAM Bank Size */
	spd_data = spd_read_byte(dev->spd_channel0[i],
					SPD_DENSITY_OF_EACH_ROW_ON_MODULE);

	/* The nibbles of the bytes are flipped between 
	 * SPD data and what Via requires */
	if (!spd_data || spd_data == 0xff)
	{
		printk(BIOS_DEBUG, "No memory in slot %d\n", i);
		return 0;
	}
	spd_data = ((spd_data >> 4) | (spd_data << 4)) & 0xff;

	printk(BIOS_DEBUG, "Found %dMB module in slot %d\n", (int)(spd_data << 6), i);

	/* Set up the DRAM registers */
	if (i)
	{
		printk(BIOS_DEBUG, "Searching for the top of the previous module\n");
		/* 0x40-0x47 are top addresses, 0x49-0x4f are base addressses */
		for(j = 1; !reg8 && j < (i * 2); j++) {
			reg8 = pci_conf1_read_config8(dev->d0f3, 0x49 + (i * 2) - j);
			printk(BIOS_DEBUG, ".");
		}
		printk(BIOS_DEBUG, "\nDone, previous module size is %dMB\n", (int)reg8);
	}

	pci_conf1_write_config8(dev->d0f3, RANK0_START + (i * 2), reg8);
	pci_conf1_write_config8(dev->d0f3, RANK0_END + (i * 2), spd_data + reg8);

	/* Set TOUD. It's easier to keep bumping 
	 * this up then to figure it out later */
	printk(BIOS_DEBUG, "Setting Top of Usable DRAM to %dMB\n", 
				(int)((spd_data + reg8) << 6));

	pci_conf1_write_config8(dev->d0f3, 0x85, (spd_data + reg8) << 2);
	pci_conf1_write_config8(dev->d0f7, 0xe5, (spd_data + reg8) << 2);

	/* TODO: needs a detection routine */
	/* if (dual sided) {
		pci_conf1_write_config8(dev->d0f3, 0x49 + (i * 2) + 1,
					spd_data + reg8);
		pci_conf1_write_config8(dev->d0f3, 0x40 + (i * 2) + 1,
					(spd_data * 2) + reg8);
	} */

	/* Enable the rank, and map it to the virtual rank of the same number. */
	reg8 = 0x80;
	reg8 |= ((i * 2) << 4);
	/* TODO: needs a detection routine */
	/* if (dual sided) reg8 |= (1 << 4) | ((i * 2) + 1); */
	pci_conf1_write_config8(dev->d0f3, 0x54 + i, reg8);

	return 1;
}

/* Transform spd byte 40's tRFC value into a decimal */
static const int byte_40_tRFC[6] = {0, 25, 33, 50, 66, 75 };
static void do_twr_trfc(struct board_info *dev, int i, int ram_cycle)
{
	u8 reg8, j;
	u16 spd_data;

	printk(BIOS_SPEW, "Calculating tWR and tRFC\n");
	/* tWR (bits 7:6) */
	/* JEDEC spec allows for decimal values, but coreboot doesn't.
	 * Convert the decimal value to an int (done more below). */
	reg8 = spd_read_byte(dev->spd_channel0[i], SPD_tWR);
	spd_data = (((reg8 & ~0x3) >> 2) * 100);
	spd_data |= ((reg8 & 0x3) * 25);
	spd_data = spd_data / (ram_cycle * 100);
	spd_data = check_timing(spd_data, 2, 5);
	reg8 = pci_conf1_read_config8(dev->d0f3, 0x61);
	if ((spd_data - 2) > (reg8 >> 6))
	{
		reg8 &= ~(0x3 << 6);
		reg8 |= ((spd_data - 2) << 6);
	}

	/* tRFC */
	spd_data = (spd_read_byte(dev->spd_channel0[i], SPD_tRFC) * 100);
	j = spd_read_byte(dev->spd_channel0[i], 40);
	if (j & 1)
		spd_data += (256 * 100);
	j = (j >> 1) & 0x7;
	spd_data |= byte_40_tRFC[j];
	spd_data = spd_data / (ram_cycle * 100);
	spd_data = check_timing(spd_data, 8, 71);
	if ((spd_data - 8) > (reg8 & 0x3f))
	{
		reg8 &= ~0x3f;
		reg8 |= (spd_data - 8);
	}
	pci_conf1_write_config8(dev->d0f3, 0x61, reg8);
}

static void do_tras_cas(struct board_info *dev, int i, int ram_cycle)
{
	u8 reg8;
	int spd_data, j;
	
	printk(BIOS_SPEW, "Calculating tRAS and CAS\n");

	/* tRAS */
	spd_data = spd_read_byte(dev->spd_channel0[i], 30);
	spd_data = (spd_data / ram_cycle);
	spd_data = check_timing(spd_data, 5, 20);

	reg8 = pci_conf1_read_config8(dev->d0f3, 0x62);
	if ((spd_data - 10) > (reg8 >> 4))
	{
		reg8 &= 0x0f;
		reg8 |= ((spd_data -10) << 4);
	}

	/* CAS Latency */
	spd_data = spd_read_byte(dev->spd_channel0[i],
			SPD_ACCEPTABLE_CAS_LATENCIES);

	j = 2;
	while(!((spd_data >> j) & 1))
	{
		j++;
	}
	/* j should now be the CAS latency, 
	 * in T for the module's rated speed */
	j = check_timing(j, 2, 5);
	if ((j - 2) > (reg8 & 0x7))
	{
		reg8 &= ~0x7;
		reg8 |= j;
	}
	pci_conf1_write_config8(dev->d0f3, 0x62, reg8);
}

static void do_trrd_trtp_twtr(struct board_info *dev, int i, int ram_cycle)
{
	u8 reg8, j;
	u16 spd_data;

	printk(BIOS_SPEW, "Calculating tRRD, tRTP, and tWTR\n");
	/* tRRD */
	reg8 = pci_conf1_read_config8(dev->d0f3, 0x63);
	j = spd_read_byte(dev->spd_channel0[i], SPD_tRRD);
	spd_data = ((j >> 2) * 100);
	spd_data |= ((j & 0x3) * 25);
	spd_data = spd_data / (ram_cycle * 100);
	spd_data = check_timing(spd_data, 2, 5);
	if ((spd_data - 2) > (reg8 >> 6))
	{
		reg8 &= 0x3f;
		reg8 |= (spd_data - 2) << 6;
	}

	/* tRTP */
	j = spd_read_byte(dev->spd_channel0[i], SPD_tRTP);
	spd_data = ((j >> 2) * 100);
	spd_data |= ((j & 0x3) * 25);
	spd_data = spd_data / (ram_cycle * 100);
	spd_data = check_timing(spd_data, 2, 3);
	if (spd_data - 2)
	{
		reg8 |= 0x8;
	}

	/* tWTR */
	j = spd_read_byte(dev->spd_channel0[i], SPD_tWTR);
	spd_data = ((j >> 2) * 100);
	spd_data |= ((j & 0x3) * 25);
	spd_data = spd_data / (ram_cycle * 100);
	spd_data = check_timing(spd_data, 2, 3);
	if (spd_data - 2)
	{
		reg8 |= 0x2;
	}
	pci_conf1_write_config8(dev->d0f3, 0x63, reg8);
}


static void do_trcd_trp(struct board_info *dev, int i, int ram_cycle)
{
	u8 reg8, j;
	u16 spd_data;

	printk(BIOS_SPEW, "Calculating tRCD and tRP\n");

	/* tRCD */
	reg8 = pci_conf1_read_config8(dev->d0f3, 0x64);
	j = spd_read_byte(dev->spd_channel0[i], SPD_tRCD);
	spd_data = ((j >> 2) * 100);
	spd_data |= ((j & 0x3) * 25);
	spd_data = spd_data / (ram_cycle * 100);
	spd_data = check_timing(spd_data, 2, 5);
	if ((spd_data - 2) > (reg8 >> 6))
	{
		reg8 &= 0x3f;
		reg8 |= (spd_data - 2) << 6;
	}

	/* CKE Minimum Pulse */
	reg8 |= 1 << 4; /* 0 = 2T, 1 = 3T */

	/* tRP (datasheet calls this tPR) */
	j = spd_read_byte(dev->spd_channel0[i], SPD_tRP);
	spd_data = ((j >> 2) * 100);
	spd_data |= ((j & 0x3) * 25);
	spd_data = spd_data / (ram_cycle * 100);
	spd_data = check_timing(spd_data, 2, 5);
	if ((spd_data - 2) > ((reg8 >> 2) & 0x3)) {
		reg8 &= ~(0x3 << 2);
		reg8 |= (spd_data - 2) << 2;
	}

	/* Exit powerdown/active powerdown to any command delay */
	reg8 |= 0x1; /* 0 = 2T, 1 = 3T */
	pci_conf1_write_config8(dev->d0f3, 0x64, reg8);
}

/* TODO: Document MA Map type registers */
static const int ma_map_shift[4] = {4, 0, 12, 8};
static void do_ma_map(struct board_info *dev, int i)
{
	u8 reg8, spd_data;

	printk(BIOS_SPEW, "calculating MA map\n");
	/* Last but not least, MA Map Type */
	spd_data = spd_read_byte(dev->spd_channel0[i], SPD_NUM_DIMM_BANKS);
	spd_data &= 0x7;
	spd_data = check_timing(spd_data, 2, 3);
	if (spd_data - 2)
		reg8 = 4;
	else
		reg8 = 0;
	spd_data = spd_read_byte(dev->spd_channel0[i], SPD_NUM_COLUMNS);
	reg8 += (spd_data - 9);

	spd_data = pci_conf1_read_config16(dev->d0f3, 0x50);
	spd_data &= ~(0xf << ma_map_shift[i]);
	spd_data |= (reg8 << (ma_map_shift[i] + 1));

	pci_conf1_write_config16(dev->d0f3, 0x50, spd_data);
	printk(BIOS_SPEW, "Setting Memory Address Map type to 0x%x\n", reg8);
}

/**
 * Set up dram size according to spd data. Eventually, DRAM timings should be 
 * done in a similar manner.
 *
 * NOTE: Via datasheets contradict themselves. Some places specify 8 ranks (4 banks),
 * other places (on the same page) say 4 ranks. This code should be able to handle
 * either situation.
 *
 * TODO: Double sided dimm support, the old code was broken. This is why odd-numbered
 * ranks are ignored.
 *
 * @param dev The northbridge devices and spd addresses.
 */
static const int ram_speeds[6] = {10, 8 /*7.5*/, 6, 5, 3};
void sdram_set_spd_registers(struct board_info *dev)
{
	int i, ram_cycle;
	u8 reg8;

	/* Table to transform the ram speed register value into a MHz value */ 
	/* TODO: multiply table by 10 to get 7.5ns timing accurately
	 * without using floating points */
	
	/* ram_cycle is the cycle time of the ram, in ns */
	reg8 = (pci_conf1_read_config8(dev->d0f2, 0x57) >> 5);
	ram_cycle = ram_speeds[reg8];

	for(i = 0; i < ARRAY_SIZE(dev->spd_channel0); i++)
	{
		printk(BIOS_SPEW, "Configuring DRAM Bank %d\n", i);
		
		/* If there's no ram, just continue to the next bank */
		if (do_ram_size(dev, i))
		{
			do_twr_trfc(dev, i, ram_cycle);
			do_tras_cas(dev, i, ram_cycle);
			do_trrd_trtp_twtr(dev, i, ram_cycle);
			do_trcd_trp(dev, i, ram_cycle);
			do_ma_map(dev, i);
		}
	}
}

/**
 * Use the DRAM timing values previously plugged into the CN700 registers to
 * calculate an MRS value that matches Via's values, defined in the CN700 Bios
 * Porting Guide. Somehow these conform to JEDEC spec, but I'm not sure how.
 *
 * CAS	BL	WR=2	WR=3	WR=4	WR=5
 *  2	4	001150	100150	101150	002150
 *  2	8	001158	100158	101158	002158
 *  3	4	0011d0	1001d0	1011d0	0021d0
 *  3	8	0011d8	1001d8	1011d8	0021d8
 *  4	4	001250	100250	101250	002250
 *  4	8	001258	100258	101258	002258
 *  5	4	0012d0	1002d0	1012d0	0022d0
 *  5	8	0012d8	1002d8	1012d8	0022d8
**/
static const int CASLength[4] = {0x150, 0x1d0, 0x250, 0x2d0};
static const int WR[4] = {0x1000, 0x100000, 0x101000, 0x2000};
static u32 find_ddr2_mrs_value(struct board_info *dev)
{
	u8 reg8;
	u32 mrs = 0x0;

	reg8 = pci_conf1_read_config8(dev->d0f3, 0x6c);
	reg8 &= 8;
	mrs |= reg8;

	reg8 = pci_conf1_read_config8(dev->d0f3, 0x62) & 0x3;
	mrs |= CASLength[reg8];

	reg8 = pci_conf1_read_config8(dev->d0f3, 0x61) >> 6;
	mrs |= WR[reg8];

	printk(BIOS_SPEW, "Mode Register Set (MRS) value is 0x%x\n", mrs);

	return mrs;
}

void ddr2_sdram_enable(struct board_info *dev)
{
	u32 reg32;

	/* 1. Apply NOP. */
	printk(BIOS_SPEW, "RAM Enable 1: Apply NOP\n");
	do_ram_command(dev, RAM_COMMAND_NOP, 0);
	udelay(200);

	/* 2. Precharge all. */
	printk(BIOS_SPEW, "RAM Enable 2: Precharge all\n");
	do_ram_command(dev, RAM_COMMAND_PRECHARGE, 0);

	/* 3. Mode register set (DLL Reset). */
	printk(BIOS_SPEW, "RAM Enable 3: Mode register set (DLL Reset)\n");
	do_ram_command(dev, RAM_COMMAND_MRS, 0x12000);
	
	/* 4. Precharge all again. */
	printk(BIOS_SPEW, "RAM Enable 4: Precharge all\n");
	do_ram_command(dev, RAM_COMMAND_PRECHARGE, 0);
	
	/* 5. Perform 8 refresh cycles (handled by do_ram_command()) */
	printk(BIOS_SPEW, "RAM Enable 5: CBR\n");
	do_ram_command(dev, RAM_COMMAND_CBR, 0);

	/* 6. Mode register set (MRS Command and OCD Calibration). */
	printk(BIOS_SPEW, "RAM Enable 6: Mode register set (OCD Calibration)\n");
	reg32 = find_ddr2_mrs_value(dev);
	do_ram_command(dev, RAM_COMMAND_MRS, reg32);
	
	/* 7. Normal operation */
	printk(BIOS_SPEW, "RAM Enable 7: Normal operation\n");
	do_ram_command(dev, RAM_COMMAND_NORMAL, 0);
	do_ram_command(dev, RAM_COMMAND_NORMAL, 0x30);
	
	/* Enable refresh. 
	 * TODO: Needs to be dynamic, 
	 * doesn't matter until ram speed change works */
	pci_conf1_write_config8(dev->d0f3, DRAM_REFRESH_COUNTER, 0x32);
	
	/* DQS Tuning: testing on a couple different boards has shown this is
	 * static, or close enough that it can be. Which is good, because the
	 * tuning function used too many registers. */
	pci_conf1_write_config8(dev->d0f3, CH_A_DQS_OUTPUT_DELAY, 0x00);
	pci_conf1_write_config8(dev->d0f3, CH_A_MD_OUTPUT_DELAY, 0x03);

	/* Enable VGA device with no memory, add memory later. We need this
	 * here to enable the actual device, otherwise it won't show up until
	 * later and CB will have a fit. */
	pci_conf1_write_config16(dev->d0f3, 0xa0, (1 << 15));
	pci_conf1_write_config16(dev->d0f3, 0xa4, 0x0010);
}
