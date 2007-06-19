/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2007 Corey Osgood <corey@slightlyhackish.com>
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
#include "i82810.h"

/*-----------------------------------------------------------------------------
Macros and definitions.
-----------------------------------------------------------------------------*/

/* Uncomment this to enable debugging output. */
// #define DEBUG_RAM_SETUP 1

/* Debugging macros. */
#if defined(DEBUG_RAM_SETUP)
#define PRINT_DEBUG(x)		print_debug(x)
#define PRINT_DEBUG_HEX8(x)	print_debug_hex8(x)
#define PRINT_DEBUG_HEX16(x)	print_debug_hex16(x)
#define PRINT_DEBUG_HEX32(x)	print_debug_hex32(x)
#define DUMPNORTH()		dump_pci_device(PCI_DEV(0, 0, 0))
#else
#define PRINT_DEBUG(x)
#define PRINT_DEBUG_HEX8(x)
#define PRINT_DEBUG_HEX16(x)
#define PRINT_DEBUG_HEX32(x)
#define DUMPNORTH()
#endif

/* DRAMT[7:5] - SDRAM Mode Select (SMS). */
#define RAM_COMMAND_SELF_REFRESH 0x0 /* IE disable refresh */
#define RAM_COMMAND_NORMAL	 0x1 /* Normal refresh, 15.6us/11.7us for 100/133MHz */
#define RAM_COMMAND_NORMAL_FR	 0x2 /* Fast refresh, 7.8us/5.85us for 100/133MHz */
#define RAM_COMMAND_NOP		 0x4
#define RAM_COMMAND_PRECHARGE	 0x5
#define RAM_COMMAND_MRS		 0x6
#define RAM_COMMAND_CBR		 0x7

/*-----------------------------------------------------------------------------
SDRAM configuration functions.
-----------------------------------------------------------------------------*/

/**
 * Send the specified RAM command to all DIMMs.
 *
 * @param Memory controller
 * @param TODO
 * @param TODO
 */
static void do_ram_command(const struct mem_controller *ctrl, uint32_t command,
			   uint32_t addr_offset, uint32_t row_offset)
{
	uint8_t reg;

	/* TODO: Support for multiple DIMMs. */

	/* Configure the RAM command. */
	reg = pci_read_config8(ctrl->d0, DRAMT);
	reg &= 0x1f;		/* Clear bits 7-5. */
	reg |= command << 5;
	pci_write_config8(ctrl->d0, DRAMT, reg);

	/* RAM_COMMAND_NORMAL affects only the memory controller and
	   doesn't need to be "sent" to the DIMMs. */
	/* if (command == RAM_COMMAND_NORMAL) return; */

	PRINT_DEBUG("    Sending RAM command 0x");
	PRINT_DEBUG_HEX8(reg);
	PRINT_DEBUG(" to 0x");
	PRINT_DEBUG_HEX32(0 + addr_offset); // FIXME
	PRINT_DEBUG("\r\n");

	/* Read from (DIMM start address + addr_offset). */
	read32(0 + addr_offset);	//first offset is always 0
	read32(row_offset + addr_offset);
}

/*-----------------------------------------------------------------------------
DIMM-independant configuration functions.
-----------------------------------------------------------------------------*/

static void spd_set_dram_size(const struct mem_controller *ctrl, uint32_t row_offset)
{
	/* The variables drp and dimm_size have to be ints since all the
	 * SMBus-related functions return ints, and its just easier this way.
	 */
	int i, drp, dimm_size;

	drp = 0x00;
	
	for (i = 0; i < DIMM_SOCKETS; i++)
	{
		/* First check if a DIMM is actually present. */
		if (smbus_read_byte(ctrl->channel0[i], 2) == 4) {
			print_debug("Found DIMM in slot ");
			print_debug_hex8(i);
			print_debug("\r\n");

			dimm_size = smbus_read_byte(ctrl->channel0[i], 31);

			/* WISHLIST: would be nice to display it as decimal? */
			print_debug("DIMM is 0x");
			print_debug_hex8(dimm_size * 4);
			print_debug("MB\r\n");

			/* The i810 can't handle DIMMs larger than 128MB per
			 * side. This will fail if the DIMM uses a
			 * non-supported DRAM tech, and can't be used until
			 * buffers are done dynamically.
			 * Note: the factory BIOS just dies if it spots
			 * this :D
			 */
			if(dimm_size > 32) {
				print_err("DIMM row sizes larger than 128MB not" 
							"supported on i810\r\n");
				print_err("Attempting to treat as 128MB DIMM\r\n");
				dimm_size = 32;
			}

			/* Set the row offset, in KBytes (should this be Kbits?) */
			/* Note that this offset is the start of the next row. */
			row_offset = (dimm_size * 4 * 1024);

			/* This is the way I was doing this, it's provided mainly
			 * as an alternative to the "new" way.
			 */

			#if 0
			/* 8MB */
			if(dimm_size == 0x2) dimm_size = 0x1;
			/* 16MB */
			else if(dimm_size == 0x4) dimm_size = 0x4;
			/* 32MB */
			else if(dimm_size == 0x8) dimm_size = 0x7;
			/* 64 MB */
			else if(dimm_size == 0x10) dimm_size = 0xa;
			/* 128 MB */
			else if(dimm_size == 0x20) dimm_size = 0xd;
			else print_debug("Ram Size not supported\r\n");
			#endif

			/* This array is provided in raminit.h, because it got
			 * extremely messy. The above way is cleaner, but
			 * doesn't support any asymetrical/odd configurations.
			 */
			dimm_size = translate_spd_to_i82810[dimm_size];

			print_debug("After translation, dimm_size is 0x");
			print_debug_hex8(dimm_size);
			print_debug("\r\n");

			/* If the DIMM is dual-sided, the DRP value is +2 */
			/* TODO: Figure out asymetrical configurations */
			if ((smbus_read_byte(ctrl->channel0[i], 127) | 0xf) == 0xff) {
				print_debug("DIMM is dual-sided\r\n");
				dimm_size += 2;
			}
		} else {
			print_debug("No DIMM found in slot ");
			print_debug_hex8(i);
			print_debug("\r\n");

			/* If there's no DIMM in the slot, set the value to 0. */
			dimm_size = 0x00;
		}

		/* Put in dimm_size to reflect the current DIMM. */
		drp |= dimm_size << (i * 4);
	}

	print_debug("DRP calculated to 0x");
	print_debug_hex8(drp);
	print_debug("\r\n");

	pci_write_config8(ctrl->d0, DRP, drp);
}

static void set_dram_timing(const struct mem_controller *ctrl)
{
	/* TODO, for now using default, hopefully safe values. */
	// pci_write_config8(ctrl->d0, DRAMT, 0x00);
}

static void set_dram_buffer_strength(const struct mem_controller *ctrl)
{
	/* TODO: This needs to be set according to the DRAM tech
	 * (x8, x16, or x32). Argh, Intel provides no docs on this!
	 * Currently, it needs to be pulled from the output of
	 * lspci -xxx Rx92
	 */
	pci_write_config16(ctrl->d0, BUFF_SC, 0x77da);
}

/*-----------------------------------------------------------------------------
Public interface.
-----------------------------------------------------------------------------*/

/**
 * TODO.
 *
 * @param Memory controller
 */
static void sdram_set_registers(const struct mem_controller *ctrl)
{
	unsigned long val;

	/* TODO */
	pci_write_config8(ctrl->d0, GMCHCFG, 0x60);

	/* PAMR: Programmable Attributes Register
	 * Every pair of bits controls an address range:
	 * 00 = Disabled, all accesses are forwarded to the ICH
	 * 01 = Read Only
	 * 10 = Write Only
	 * 11 = Read/Write

	 * Bit	Range
	 * 7:6	000F0000 - 000FFFFF
	 * 5:4	000E0000 - 000EFFFF
	 * 3:2	000D0000 - 000DFFFF
	 * 1:0	000C0000 - 000CFFFF
	 */

	/* Ideally, this should be R/W for as many ranges as possible. */
	pci_write_config8(ctrl->d0, PAM, 0xff);

	/* Enabling the VGA Framebuffer currently screws up the rest of the boot.
	 * Disable for now */
	
	/* Enable 1MB framebuffer. */
	//pci_write_config8(ctrl->d0, SMRAM, 0xC0);

	//val = pci_read_config16(ctrl->d0, MISSC);
	/* Preserve reserved bits. */
	//val &= 0xff06;
	/* Set graphics cache window to 32MB, no power throttling. */
	//val |= 0x0001;
	//pci_write_config16(ctrl->d0, MISSC, val);

	//val = pci_read_config8(ctrl->d0, MISSC2);
	/* Enable graphics palettes and clock gating (not optional!) */
	//val |= 0x06;
	//pci_write_config8(ctrl->d0, MISSC2, val);
}

/**
 * TODO.
 *
 * @param Memory controller
 */
static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
	/* spd_set_dram_size() moved into sdram_enable() to prevent having
	 * to pass a variable between here and there.
	 */
	set_dram_buffer_strength(ctrl);

	set_dram_timing(ctrl);
}

/**
 * Enable SDRAM.
 *
 * @param Number of controllers
 * @param Memory controller
 */
static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;

	/* Todo: this will currently work with either one dual sided or two
	 * single sided DIMMs. Needs to work with 2 dual sided DIMMs in the
	 * long run.
	 */
	uint32_t row_offset;

	spd_set_dram_size(ctrl, row_offset); 

	/* 1. Apply NOP. */
	PRINT_DEBUG("RAM Enable 1: Apply NOP\r\n");
	do_ram_command(ctrl, RAM_COMMAND_NOP, 0, row_offset);
	udelay(200);

	/* 2. Precharge all. Wait tRP. */
	PRINT_DEBUG("RAM Enable 2: Precharge all\r\n");
	do_ram_command(ctrl, RAM_COMMAND_PRECHARGE, 0, row_offset);
	udelay(1);

	/* 3. Perform 8 refresh cycles. Wait tRC each time. */
	PRINT_DEBUG("RAM Enable 3: CBR\r\n");
	do_ram_command(ctrl, RAM_COMMAND_CBR, 0, row_offset);
	for (i = 0; i < 8; i++) {
		read32(0);
		read32(row_offset);
		udelay(1);
	}

	/* 4. Mode register set. Wait two memory cycles. */
	PRINT_DEBUG("RAM Enable 4: Mode register set\r\n");
	do_ram_command(ctrl, RAM_COMMAND_MRS, 0x1d0, row_offset);
	udelay(2);

	/* 5. Normal operation (enables refresh) */
	PRINT_DEBUG("RAM Enable 5: Normal operation\r\n");
	do_ram_command(ctrl, RAM_COMMAND_NORMAL, 0, row_offset);
	udelay(1);

	PRINT_DEBUG("Northbridge following SDRAM init:\r\n");
	DUMPNORTH();
}
