/*
 * This file is part of the coreboot project.
 *
  * Copyright (C) 2011-2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Inspired from the EPIA-M700
 */

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include <console/console.h>
#include <lib.h>
#include "cpu/x86/bist.h"
#include <string.h>

#include "northbridge/via/vx900/early_vx900.h"
#include "northbridge/via/vx900/raminit.h"
#include "superio/fintek/f81865f/f81865f_early_serial.c"

#define SERIAL_DEV PNP_DEV(0x4e, 0x10)

/* cache_as_ram.inc jumps to here. */
void main(unsigned long bist)
{
	/* Enable multifunction bit for northbridge.
	 * This enables the PCI configuration spaces of D0F1 to D0F7 to be
	 * accessed */
	pci_write_config8(PCI_DEV(0, 0, 0), 0x4f, 0x01);

	f81865f_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();
	print_debug("Console initialized. \n");

	/* Halt if there was a built-in self test failure. */
	report_bist_failure(bist);

	/* x86 cold boot I/O cmd. */
	enable_smbus();

	/* If this works, then SMBUS is up and running */
	//dump_spd_data();

	/* Now we can worry about raminit.
	 * This board only has DDR3, so no need to worry about which DRAM type
	 * to use */
	dimm_layout dimms = {{0x50, 0x51, SPD_END_LIST}};
	vx900_init_dram_ddr3(&dimms);

	//ram_check(0, 0x80);
	ram_check((1<<20), 0x80);
	ram_check((1<<26), 0x80);


	print_debug("We passed RAM verify\n");

	return;

}
