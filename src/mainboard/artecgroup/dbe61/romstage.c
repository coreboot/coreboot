/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <stdlib.h>
#include <console/console.h>
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/lxdef.h>
#include "southbridge/amd/cs5536/cs5536.h"
#include "spd_table.h"
#include <spd.h>
#include "southbridge/amd/cs5536/early_smbus.c"
#include "southbridge/amd/cs5536/early_setup.c"
#include "northbridge/amd/lx/raminit.h"

int spd_read_byte(unsigned int device, unsigned int address)
{
	int i;

	if (device == DIMM0) {
		for (i=0; i < (ARRAY_SIZE(spd_table)); i++) {
			if (spd_table[i].address == address) {
				return spd_table[i].data;
			}
		}
	}

	/* returns 0xFF on any failures */
	return 0xFF;
}

#include "northbridge/amd/lx/pll_reset.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/geode_lx/cpureginit.c"
#include "cpu/amd/geode_lx/syspreinit.c"
#include "cpu/amd/geode_lx/msrinit.c"

void main(unsigned long bist)
{

	msr_t msr;
	static const struct mem_controller memctrl[] = {
		{.channel0 = {DIMM0, DIMM1}}
	};

	SystemPreInit();
	msr_init();

	cs5536_early_setup();

	/* NOTE: must do this AFTER the early_setup!
	 * it is counting on some early MSR setup
	 * for cs5536
	 */
	/* cs5536_disable_internal_uart	 disable them. Set them up now... */
	cs5536_setup_onchipuart(2); /* dbe61 uses UART2 as COM1 */
	/* set address to 3F8 */
	msr = rdmsr(MDD_LEG_IO);
	msr.lo |= 0x7 << 20;
	wrmsr(MDD_LEG_IO, msr);

	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	pll_reset();

	cpuRegInit(0, DIMM0, DIMM1, DRAM_TERMINATED);

	sdram_initialize(1, memctrl);

	/* Dump memory configuration. */
#if 0
	msr = rdmsr(MC_CF07_DATA);
	print_debug("MC_CF07_DATA: ");
	print_debug_hex32(MC_CF07_DATA);
	print_debug(" value is: ");
	print_debug_hex32(msr.hi);
	print_debug(":");
	print_debug_hex32(msr.lo);
	print_debug(" \n");

	msr = rdmsr(MC_CF1017_DATA);
	print_debug("MC_CF1017_DATA: ");
	print_debug_hex32(MC_CF1017_DATA);
	print_debug(" value is: ");
	print_debug_hex32(msr.hi);
	print_debug(":");
	print_debug_hex32(msr.lo);
	print_debug(" \n");

	msr = rdmsr(MC_CF8F_DATA);
	print_debug("MC_CF8F_DATA: ");
	print_debug_hex32(MC_CF8F_DATA);
	print_debug(" value is: ");
	print_debug_hex32(msr.hi);
	print_debug(":");
	print_debug_hex32(msr.lo);
	msr = rdmsr(MC_CF8F_DATA);
	print_debug(" \n");
#endif
}
