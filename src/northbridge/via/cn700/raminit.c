/*
 * This file is part of the coreboot project.
 *
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <spd.h>
#include <sdram_mode.h>
#include <delay.h>
#include "cn700.h"

//#define DEBUG_RAM_SETUP 1

#ifdef DEBUG_RAM_SETUP
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

static void do_ram_command(device_t dev, u8 command, u32 addr_offset)
{
	u8 reg;

	/* TODO: Support for multiple DIMMs. */

	reg = pci_read_config8(dev, DRAM_MISC_CTL);
	reg &= 0xf8;		/* Clear bits 2-0. */
	reg |= command;
	pci_write_config8(dev, DRAM_MISC_CTL, reg);

	PRINT_DEBUG_MEM("    Sending RAM command 0x");
	PRINT_DEBUG_MEM_HEX8(reg);
	PRINT_DEBUG_MEM(" to 0x");
	PRINT_DEBUG_MEM_HEX32(0 + addr_offset);
	PRINT_DEBUG_MEM("\r\n");

	read32(0 + addr_offset);
}


/**
 * Configure the bus between the cpu and the northbridge. This might be able to 
 * be moved to post-ram code in the future. For the most part, these registers
 * should not be messed around with. These are too complex to explain short of
 * copying the datasheets into the comments, but most of these values are from
 * the BIOS Porting Guide, so they should work on any board. If they don't,
 * try the values from your factory BIOS.
 *
 * TODO: Changing the DRAM frequency doesn't work (hard lockup)
 *
 * @param dev The northbridge's CPU Host Interface (D0F2)
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
	pci_write_config8(dev, 0x54, 0x10);
	pci_write_config8(dev, 0x55, 0x16);
	/* Write Policy */
	pci_write_config8(dev, 0x56, 0x01);
	/* Miscellaneous Control */
	/* DRAM Operating Frequency (Bits 7:5)
	 *	000 : 100MHz	001 : 133MHz
	 *	010 : 166MHz	011 : 200MHz
	 *	100 : 266MHz	101 : 333MHz
	 *	110/111 : Reserved */
	//pci_write_config8(dev, 0x57, 0x60);//set 200MHz dram clock
	/* CPU Miscellaneous Control */
	pci_write_config8(dev, 0x59, 0x60);
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
	pci_write_config8(dev, 0x67, 0x70);
		
	/* Host Bus IO Circuit (See datasheet) */
	/* Host Address Pullup/down Driving */
	pci_write_config8(dev, 0x70, 0x33);
	pci_write_config8(dev, 0x71, 0x00);
	pci_write_config8(dev, 0x72, 0x33);
	pci_write_config8(dev, 0x73, 0x00);
	/* Miscellaneous Control */
	pci_write_config8(dev, 0x74, 0x00);
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
	pci_write_config8(dev, 0x7b, 0x00);
	/* Address CKG Clock Rising/Falling Time Control */
	pci_write_config8(dev, 0x7c, 0x00);
	/* Undefined (can't remember why I did this) */
	pci_write_config8(dev, 0x7d, 0x6d);	
}

/**
 * Set up various ram and other control registers statically. Some of these may 
 * not be needed, other should be done with spd info, but that's a project for
 * the future
 */
static void sdram_set_registers(const struct mem_controller *ctrl)
{
	/* DQ/DQS Strength Control */
	pci_write_config8(ctrl->d0f3, 0xd0, 0x88);
	pci_write_config8(ctrl->d0f3, 0xd1, 0x8b);
	pci_write_config8(ctrl->d0f3, 0xd2, 0x89);
	/* SMM and APIC Decoding */
	pci_write_config8(ctrl->d0f3, 0x86, 0x2d);
	
	/* Driving selection */
	/* DQ / DQS ODT Driving and Range Select */
	pci_write_config8(ctrl->d0f3, 0xd5, 0x8a);
	/* Memory Pads Driving and Range Select */
	pci_write_config8(ctrl->d0f3, 0xd6, 0xaa);
	/* DRAM Driving – Group DQS */
	pci_write_config8(ctrl->d0f3, 0xe0, 0xee);
	/* DRAM Driving – Group DQ (DQ, MPD, DQM) */
	pci_write_config8(ctrl->d0f3, 0xe2, 0xac);//ba
	/* DRAM Driving – Group CS */
	pci_write_config8(ctrl->d0f3, 0xe4, 0x66);
	/* DRAM Driving – Group MA */
	pci_write_config8(ctrl->d0f3, 0xe8, 0x86);
	/* DRAM Driving – Group MCLK */
	pci_write_config8(ctrl->d0f3, 0xe6, 0xaa);

	/* ODT (some are set with driving select above) */
	/* Memory Pad ODT Pullup / Pulldown Control */
	pci_write_config8(ctrl->d0f3, 0xd4, 0x0a);
	/* Memory Ranks ODT Lookup Table */
	pci_write_config8(ctrl->d0f3, 0xd8, 0x00);//was 1
	/* Compensation Control */
	pci_write_config8(ctrl->d0f3, 0xd3, 0x89);//enable auto compensation
	
	/* MCLKO Phase Control */
	pci_write_config8(ctrl->d0f3, 0x91, 0x02);
	/* CS/CKE Clock Phase Control */
	pci_write_config8(ctrl->d0f3, 0x92, 0x06);
	/* SCMD/MA Clock Phase Control */
	pci_write_config8(ctrl->d0f3, 0x93, 0x07);
	/* Channel A DQS Input Capture Range Control */
	pci_write_config8(ctrl->d0f3, 0x78, 0x83);
	/* DQS Input Capture Range Control */
	/* Set in accordance with the BIOS update note */
	pci_write_config8(ctrl->d0f3, 0x7a, 0x00);
	/* DQS Input Delay Offset Control */
	pci_write_config8(ctrl->d0f3, 0x7c, 0x00);
	/* SDRAM ODT Control */
	pci_write_config8(ctrl->d0f3, 0xda, 0x80);
	/* DQ/DQS CKG Output Delay Control - I */
	pci_write_config8(ctrl->d0f3, 0xdc, 0xff);
	/* DQ/DQS CKG Output Delay Control - II */
	pci_write_config8(ctrl->d0f3, 0xdd, 0xff);
	/* DQS / DQ CKG Duty Cycle Control */
	pci_write_config8(ctrl->d0f3, 0xec, 0x88);
	/* MCLK Output Duty Control */
	pci_write_config8(ctrl->d0f3, 0xee, 0x00);
	pci_write_config8(ctrl->d0f3, 0xed, 0x10);
	/* DQS CKG Input Delay Control */
	pci_write_config8(ctrl->d0f3, 0xef, 0x10);

	pci_write_config8(ctrl->d0f3, 0x77, 0x9d);
	pci_write_config8(ctrl->d0f3, 0x79, 0x83);
	pci_write_config16(ctrl->d0f3, 0x88, 0x0020);
	
	pci_write_config8(ctrl->d0f4, 0xa7, 0x80);

	/* VLink Control */
	pci_write_config8(ctrl->d0f7, 0xb0, 0x05);
	pci_write_config8(ctrl->d0f7, 0xb1, 0x01);
	
	/* Memory base */
	pci_write_config16(ctrl->d1f0, 0x20, 0xfb00);
	/* Memory limit */
	pci_write_config16(ctrl->d1f0, 0x22, 0xfcf0);
	/* Prefetch memory base */
	pci_write_config16(ctrl->d1f0, 0x24, 0xf400);
	/* Prefetch memory limit */
	pci_write_config16(ctrl->d1f0, 0x26, 0xf7f0);
	/* PCI to PCI bridge control */
	pci_write_config16(ctrl->d1f0, 0x3e, 0x0008);
	
	/* CPU to PCI flow control 1 */
	pci_write_config8(ctrl->d1f0, 0x40, 0x83);
	pci_write_config8(ctrl->d1f0, 0x41, 0xc3);//clear reset error, set to 43
	pci_write_config8(ctrl->d1f0, 0x42, 0xe2);
	pci_write_config8(ctrl->d1f0, 0x43, 0x44);
	pci_write_config8(ctrl->d1f0, 0x44, 0x34);
	pci_write_config8(ctrl->d1f0, 0x45, 0x72);

	/* Disable cross bank/multi page mode */
	pci_write_config8(ctrl->d0f3, DDR_PAGE_CTL, 0x80);
	pci_write_config8(ctrl->d0f3, DRAM_REFRESH_COUNTER, 0x00);

	/* Set WR=5 and RFC */
	pci_write_config8(ctrl->d0f3, 0x61, 0xc7);
	/* Set CAS=5 */
	pci_write_config8(ctrl->d0f3, 0x62, 0xaf);
	pci_write_config8(ctrl->d0f3, 0x63, 0xca);
	/* Set to DDR2 sdram, BL=8 (0xc8, 0xc0 for bl=4) */
	pci_write_config8(ctrl->d0f3, 0x6c, 0xc8);
	/* Allow manual dll reset */
	pci_write_config8(ctrl->d0f3, 0x6b, 0x10);
	
	pci_write_config8(ctrl->d0f3, 0x6e, 0x89);
	pci_write_config8(ctrl->d0f3, 0x67, 0x50);
	pci_write_config8(ctrl->d0f3, 0x65, 0xd9);
	
	/* Only enable bank 1, for now */
	/* TODO: Multiple, dynamically controlled bank enables */
	pci_write_config8(ctrl->d0f3, 0x54, 0x80);
	pci_write_config8(ctrl->d0f3, 0x55, 0x00);
	
	/* Set to 2T, MA Map type 1. 
	 * TODO: Needs to become dynamic */
	pci_write_config16(ctrl->d0f3, 0x50, 0x0020);

	/* BA0-2 Selection. Don't mess with */
	pci_write_config8(ctrl->d0f3, 0x52, 0x33);
	pci_write_config8(ctrl->d0f3, 0x53, 0x3f);
	
	/* Disable bank interleaving. This feature seems useless anyways */
	pci_write_config32(ctrl->d0f3, 0x58, 0x00000000);
	pci_write_config8(ctrl->d0f3, 0x88, 0x08);

	/* Some DQS control stuffs */
	pci_write_config8(ctrl->d0f3, 0x74, 0x04);
	pci_write_config8(ctrl->d0f3, 0x75, 0x04);
	pci_write_config8(ctrl->d0f3, 0x76, 0x00);
}

/**
 * Set up dram size according to spd data. Eventually, DRAM timings should be 
 * done in a similar manner.
 *
 * @param ctrl The northbridge devices and spd addresses.
 */
static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
	u8 spd_data, spd_data2;
	
	/* DRAM Bank Size */
	spd_data = spd_read_byte(ctrl->channel0[0],
					SPD_DENSITY_OF_EACH_ROW_ON_MODULE);
	/* I know this seems weird. Blame JEDEC/Via. */
	if(spd_data >= 0x10)
		spd_data = spd_data >> 1;
	else
		spd_data = spd_data << 1;

	/* Check for double sided dimm and adjust size accordingly */
	spd_data2 = spd_read_byte(ctrl->channel0[0], SPD_NUM_BANKS_PER_SDRAM);
	/* There should be 4 banks on a single sided dimm, 
	 * or 8 on a dual sided one */
	spd_data = spd_data * (spd_data2 / 4);
	pci_write_config8(ctrl->d0f3, 0x40, spd_data);
	/* TODO: The rest of the DIMMs */
}

static void sdram_enable(device_t dev)
{
	int i;

	/* 1. Apply NOP. */
	PRINT_DEBUG_MEM("RAM Enable 1: Apply NOP\r\n");
	do_ram_command(dev, RAM_COMMAND_NOP, 0);
	udelay(200);

	/* 2. Precharge all. */
	PRINT_DEBUG_MEM("RAM Enable 2: Precharge all\r\n");
	do_ram_command(dev, RAM_COMMAND_PRECHARGE, 0);

	/* 3. Mode register set. */
	PRINT_DEBUG_MEM("RAM Enable 4: Mode register set\r\n");
	do_ram_command(dev, RAM_COMMAND_MRS, 0x2000);//enable dll
	do_ram_command(dev, RAM_COMMAND_MRS, 0x800);//reset dll
	
	/* 4. Precharge all again. */
	PRINT_DEBUG_MEM("RAM Enable 2: Precharge all\r\n");
	do_ram_command(dev, RAM_COMMAND_PRECHARGE, 0);
	
	/* 5. Perform 8 refresh cycles. Wait tRC each time. */
	PRINT_DEBUG_MEM("RAM Enable 3: CBR\r\n");
	do_ram_command(dev, RAM_COMMAND_CBR, 0);
	/* First read is actually done by do_ram_command */
	for(i = 0; i < 7; i++) {
		udelay(100);
		read32(0);
	}

	/* 6. Mode register set. */
	PRINT_DEBUG_MEM("RAM Enable 4: Mode register set\r\n");
	//safe value for now, BL=8, WR=5, CAS=5
	/* (E)MRS values are from the BPG. No direct explanation is given, but 
	 * they should somehow conform to the JEDEC DDR2 SDRAM Specification
	 * (JESD79-2C). */
	do_ram_command(dev, RAM_COMMAND_MRS, 0x0022d8);
	
	/* 7. Mode register set. */
	PRINT_DEBUG_MEM("RAM Enable 4: Mode register set\r\n");
	do_ram_command(dev, RAM_COMMAND_MRS, 0x21c20);//default OCD calibration
	do_ram_command(dev, RAM_COMMAND_MRS, 0x20020);//exit calibration mode
	
	/* 8. Normal operation */
	PRINT_DEBUG_MEM("RAM Enable 5: Normal operation\r\n");
	do_ram_command(dev, RAM_COMMAND_NORMAL, 0);
	
	/* Enable multipage mode. */
	pci_write_config8(dev, DDR_PAGE_CTL, 0x83);
	/* Enable refresh. */
	pci_write_config8(dev, DRAM_REFRESH_COUNTER, 0x32);
	
	/* DQS Tuning: testing on a couple different boards has shown this is
	 * static, or close enough that it can be. Which is good, because the
	 * tuning function used too many registers. */
	pci_write_config8(dev, CH_A_DQS_OUTPUT_DELAY, 0x00);
	pci_write_config8(dev, CH_A_MD_OUTPUT_DELAY, 0x03);

	/* Enable VGA device with no memory, add memory later. We need this
	 * here to enable the actual device, otherwise it won't show up until
	 * later and LB will have a fit. */
	pci_write_config16(dev, 0xa0, (1 << 15));
	pci_write_config16(dev, 0xa4, 0x0010);
}
