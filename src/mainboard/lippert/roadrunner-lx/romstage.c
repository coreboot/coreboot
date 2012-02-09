/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 LiPPERT Embedded Computers GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* Based on romstage.c from AMD's DB800 and DBM690T mainboards. */

#include <stdlib.h>
#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include <console/console.h>
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/lxdef.h>
#include "southbridge/amd/cs5536/cs5536.h"
#include <spd.h>
#include "southbridge/amd/cs5536/early_smbus.c"
#include "southbridge/amd/cs5536/early_setup.c"
#include "superio/ite/it8712f/early_serial.c"

#define ManualConf 1		/* No automatic strapped PLL config */
#define PLLMSRhi 0x0000049C	/* Manual settings for the PLL */
#define PLLMSRlo 0x00DE6001

static inline int spd_read_byte(unsigned int device, unsigned int address)
{
	if (device != DIMM0)
		return 0xFF;	/* No DIMM1, don't even try. */

	return smbus_read_byte(device, address);
}

#include "northbridge/amd/lx/raminit.h"
#include "northbridge/amd/lx/pll_reset.c"
#include "northbridge/amd/lx/raminit.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/geode_lx/cpureginit.c"
#include "cpu/amd/geode_lx/syspreinit.c"
#include "cpu/amd/geode_lx/msrinit.c"

static const u16 sio_init_table[] = {	// hi=data, lo=index
	0x0707,		// select LDN 7 (GPIO, SPI, watchdog, ...)
	0x1E2C,		// disable ATXPG; VIN6,FAN4/5,VIN3 enabled, VIN7 internal
	0x1423,		// don't delay PoWeROK1/2 - triggers 2nd reset
	0x9072,		// watchdog triggers PWROK, counts seconds
#if !CONFIG_USE_WATCHDOG_ON_BOOT
	0x0073, 0x0074,	// disarm watchdog by changing 56 s timeout to 0
#endif
	0xBF25, 0x372A, 0xF326, // select GPIO function for most pins
	0xBF27, 0xFF28, 0x2529, // (GP36=FAN_CTL3, GP13=PWROK1)
	0x46B8, 0x0CB9,	// enable pullups on RS485_EN
	0x36C0,		// enable Simple-I/O for GP15,14,12,11= LIVE_LED, WD_ACTIVE, RS485_EN2,1
	0xFFC3,		// enable Simple-I/O for GP47-40 (GPIOs on Supervisory Connector)
	0x26C8,		// config GP15,12,11 as output; GP14 as input
	0x2DF5,		// map Hw Monitor Thermal Output to GP55
	0x0DF8,		// map GP LED Blinking 1 to GP15=LIVE_LED (deactivate Simple-I/O to use)
};

/* Early mainboard specific GPIO setup. */
static void mb_gpio_init(void)
{
	int i;

	/* Init Super I/O WDT, GPIOs. Done early, WDT init may trigger reset! */
	it8712f_enter_conf();
	for (i = 0; i < ARRAY_SIZE(sio_init_table); i++) {
		u16 val = sio_init_table[i];
		outb((u8)val, SIO_INDEX);
		outb(val >> 8, SIO_DATA);
	}
	it8712f_exit_conf();
}

void main(unsigned long bist)
{

	static const struct mem_controller memctrl[] = {
		{.channel0 = {DIMM0, DIMM1}}
	};

	SystemPreInit();
	msr_init();

	cs5536_early_setup();

	/*
	 * Note: must do this AFTER the early_setup! It is counting on some
	 * early MSR setup for CS5536.
	 */
	it8712f_enable_serial(0, CONFIG_TTYS0_BASE); // Does not use its 1st parameter
	mb_gpio_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	pll_reset(ManualConf);

	cpuRegInit(0, DIMM0, DIMM1, DRAM_TERMINATED);

	sdram_initialize(1, memctrl);

	/* Memory is setup. Return to cache_as_ram.inc and continue to boot. */
}
