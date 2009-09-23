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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#define ASSEMBLY 1

#include <stdint.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/hlt.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "lib/ramtest.c"
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/lxdef.h>
#include <cpu/amd/geode_post_code.h>
#include "southbridge/amd/cs5536/cs5536.h"

#define POST_CODE(x) outb(x, 0x80)
#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

#include "southbridge/amd/cs5536/cs5536_early_smbus.c"
#include "southbridge/amd/cs5536/cs5536_early_setup.c"
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"

static inline int spd_read_byte(unsigned int device, unsigned int address)
{
	return smbus_read_byte(device, address);
}

#define ManualConf 1		/* Do automatic strapped PLL config */
//#define PLLMSRhi 0x0000059C /* CPU and GLIU mult/div 500/400*/
//#define PLLMSRhi 0x0000049C /* CPU and GLIU mult/div 500/333*/
#define PLLMSRhi 0x0000039C /* CPU and GLIU mult/div 500/266*/
//0x0000059C 0000 0000 0000 0000 0000 |0101 1|0|01 110|0
/* Hold Count - how long we will sit in reset */
#define PLLMSRlo 0x00DE6000

#define DIMM0 0xA0
#define DIMM1 0xA2

#include "northbridge/amd/lx/raminit.h"
#include "northbridge/amd/lx/pll_reset.c"
#include "northbridge/amd/lx/raminit.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/model_lx/cpureginit.c"
#include "cpu/amd/model_lx/syspreinit.c"

static void msr_init(void)
{
	msr_t msr;

	/* Setup access to the cache for under 1MB. */
	msr.hi = 0x24fffc02;
	msr.lo = 0x1000A000;	/* 0-A0000 write back */
	wrmsr(CPU_RCONF_DEFAULT, msr);

	msr.hi = 0x0;		/* Write back */
	msr.lo = 0x0;
	wrmsr(CPU_RCONF_A0_BF, msr);
	wrmsr(CPU_RCONF_C0_DF, msr);
	wrmsr(CPU_RCONF_E0_FF, msr);

	/* Setup access to the cache for under 640K. Note MC not setup yet. */
	msr.hi = 0x20000000;
	msr.lo = 0xfff80;
	wrmsr(MSR_GLIU0 + 0x20, msr);

	msr.hi = 0x20000000;
	msr.lo = 0x80fffe0;
	wrmsr(MSR_GLIU0 + 0x21, msr);

	msr.hi = 0x20000000;
	msr.lo = 0xfff80;
	wrmsr(MSR_GLIU1 + 0x20, msr);

	msr.hi = 0x20000000;
	msr.lo = 0x80fffe0;
	wrmsr(MSR_GLIU1 + 0x21, msr);
}

static void mb_gpio_init(void)
{
	/* Early mainboard specific GPIO setup. */
}

void cache_as_ram_main(void)
{
	POST_CODE(0x01);

	static const struct mem_controller memctrl[] = {
		{.channel0 = {(0xa << 3) | 0, (0xa << 3) | 1}}
	};

	SystemPreInit();
	msr_init();

	cs5536_early_setup();

	/* Note: must do this AFTER the early_setup! It is counting on some
	 * early MSR setup for CS5536.
	 */
	w83627hf_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	mb_gpio_init();
	uart_init();
	console_init();

	pll_reset(ManualConf);

	cpuRegInit();

	sdram_initialize(1, memctrl);

	/* ram_check(0, 640 * 1024); */

	/* Memory is setup. Return to cache_as_ram.inc and continue to boot. */
	return;
}
