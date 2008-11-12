/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
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

#define _MAINOBJECT

#include <mainboard.h>
#include <config.h>
#include <types.h>
#include <lib.h>
#include <console.h>
#include <cpu.h>
#include <globalvars.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <amd/k8/k8.h>
#include <mc146818rtc.h>
#include <spd.h>

#define RC0 ((1<<0)<<8)

#define DIMM0 0x50
#define DIMM1 0x51

/* this code is very mainboard dependent, sadly. */
/**
 * no op at present
 */
static void memreset_setup(void)
{
}

/**
 * this is a no op on this platform.
 */
void memreset(int controllers, const struct mem_controller *ctrl)
{
}

void activate_spd_rom(const struct mem_controller *ctrl)
{
}

/**
 * read a byte from spd.
 * @param device device to read from
 * @param address address in the spd ROM
 * @return the value of the byte at that address.
 */
u8 spd_read_byte(u16 device, u8 address)
{
	int do_smbus_read_byte(u16 device, u16 address);
	return do_smbus_read_byte(device, address);
}

/**
  * main for initram for the AMD DBM690T
 * @param init_detected Used to indicate that we have been started via init
 * @returns 0 on success
 * The purpose of this code is to not only get ram going, but get any other cpus/cores going.
 * The two activities are very tightly connected and not really seperable.
 *
  */
/*
 * init_detected is used to determine if we did a soft reset as required by a reprogramming of the
 * hypertransport links. If we did this kind of reset, bit 11 will be set in the MTRRdefType_MSR MSR.
 * That may seem crazy, but there are not lots of places to hide a bit when the CPU does a reset.
 * This value is picked up in assembly, or it should be.
 */
int main(void)
{
	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG, "soft reset detected.\n");
		boot_mode = 1;
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i945_early_initialization();

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();
	
#if DEFAULT_CONSOLE_LOGLEVEL > 8
	dump_spd_registers();
#endif

	sdram_initialize(boot_mode);

	/* Perform some initialization that must run before stage2 */
	early_ich7_init();

	/* This should probably go away. Until now it is required 
	 * and mainboard specific 
	 */
	rcba_config();

	/* Chipset Errata! */
	fixup_i945_errata();

	/* Initialize the internal PCIe links before we go into stage2 */
	i945_late_initialization();

#if DEFAULT_CONSOLE_LOGLEVEL > 8
#if defined(DEBUG_RAM_SETUP)
	sdram_dump_mchbar_registers();
#endif

	{
		/* This will not work if TSEG is in place! */
		u32 tom = pci_conf1_read_config32(PCI_BDF(0,2,0), 0x5c);

		printk(BIOS_DEBUG, "TOM: 0x%08x\n", tom);
		ram_check(0x00000000, 0x000a0000);
		ram_check(0x00100000, tom);
	}
#endif
	MCHBAR16(SSKPD) = 0xCAFE;
}
