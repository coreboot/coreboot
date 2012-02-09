/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Nils Jacobs
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <console/console.h>
#include <lib.h>
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/gx2def.h>
#include <spd.h>
#include "southbridge/amd/cs5536/early_smbus.c"
#include "southbridge/amd/cs5536/early_setup.c"

static inline int spd_read_byte(unsigned int device, unsigned int address)
{
	if (device != DIMM0)
		return 0xFF;	/* No DIMM1, don't even try. */

	return smbus_read_byte(device, address);
}

#include "northbridge/amd/gx2/raminit.h"
#include "northbridge/amd/gx2/pll_reset.c"
#include "northbridge/amd/gx2/raminit.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/geode_gx2/cpureginit.c"
#include "cpu/amd/geode_gx2/syspreinit.c"
#include "cpu/amd/geode_lx/msrinit.c"

void main(unsigned long bist)
{
	static const struct mem_controller memctrl [] = {
		{.channel0 = {DIMM0, DIMM1}}
	};

	SystemPreInit();

	cs5536_early_setup();

	/* cs5536_disable_internal_uart	 disable them. Set them up now... */
	cs5536_setup_onchipuart(1);

	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	pll_reset();

	cpuRegInit();
	print_err("done cpuRegInit\n");

	sdram_initialize(1, memctrl);
	print_err("ram setup done\n");

	msr_init();
}
