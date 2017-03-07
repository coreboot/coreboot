/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <console/console.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627hf/w83627hf.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/car.h>
#include <cpu/amd/gx2def.h>
#include <spd.h>
#include "southbridge/amd/cs5536/early_smbus.c"
#include "southbridge/amd/cs5536/early_setup.c"

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

static inline int spd_read_byte(unsigned device, unsigned address)
{
 	if (device != DIMM0)
		return 0xFF;	/* No DIMM1, don't even try. */

	return smbus_read_byte(device, address);
}

#include <northbridge/amd/gx2/raminit.h>
#include "northbridge/amd/gx2/pll_reset.c"
#include "northbridge/amd/gx2/raminit.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/geode_gx2/cpureginit.c"
#include "cpu/amd/geode_gx2/syspreinit.c"
#include "cpu/amd/geode_lx/msrinit.c"

void asmlinkage mainboard_romstage_entry(unsigned long bist)
{
	static const struct mem_controller memctrl [] = {
		{.channel0 = {DIMM0, DIMM1}}
	};

	SystemPreInit();

	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	console_init();

	cs5536_early_setup();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	pll_reset();

	cpuRegInit();
	printk(BIOS_ERR, "done cpuRegInit\n");

	sdram_initialize(1, memctrl);

	msr_init();
}
