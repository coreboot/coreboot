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

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define POST_CODE(x) outb(x, 0x80)
#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

#include "southbridge/amd/cs5536/cs5536_early_smbus.c"
#include "southbridge/amd/cs5536/cs5536_early_setup.c"
#include "superio/winbond/w83627hf/w83627hf_early_serial.c"

static inline int spd_read_byte(unsigned int device, unsigned int address)
{
	return smbus_read_byte(device, address);
}

#define ManualConf 0		/* Do automatic strapped PLL config */
#define PLLMSRhi 0x00001490	/* Manual settings for the PLL */
#define PLLMSRlo 0x02000030
#define DIMM0 0xA0
#define DIMM1 0xA2

#include "northbridge/amd/lx/raminit.h"
#include "northbridge/amd/lx/pll_reset.c"
#include "northbridge/amd/lx/raminit.c"
#include "lib/generic_sdram.c"
#include "cpu/amd/model_lx/cpureginit.c"
#include "cpu/amd/model_lx/syspreinit.c"

struct msrinit {
            u32 msrnum;
            msr_t msr;
      };

static const struct msrinit msr_table[] = 
{
       {CPU_RCONF_DEFAULT, {.hi = 0x24fffc02,.lo = 0x1000A000}}, /* Setup access to cache under 1MB.
                                                                  * Rom Properties: Write Serialize, WriteProtect.
                                                                  * RomBase: 0xFFFC0
                                                                  * SysTop to RomBase Properties: Write Serialize, Cache Disable.
                                                                  * SysTop: 0x000A0 
                                                                  * System Memory Properties:  (Write Back) */
       {CPU_RCONF_A0_BF,   {.hi = 0x00000000,.lo = 0x00000000}}, /* 0xA0000-0xBFFFF : (Write Back) */
       {CPU_RCONF_C0_DF,   {.hi = 0x00000000,.lo = 0x00000000}}, /* 0xC0000-0xDFFFF : (Write Back) */
       {CPU_RCONF_E0_FF,   {.hi = 0x00000000,.lo = 0x00000000}}, /* 0xE0000-0xFFFFF : (Write Back) */
       
       /* Setup access to memory under 1MB. Note: VGA hole at 0xA0000-0xBFFFF */
       {MSR_GLIU0_BASE1,   {.hi = 0x20000000,.lo = 0x000fff80}}, // 0x00000-0x7FFFF
       {MSR_GLIU0_BASE2,   {.hi = 0x20000000,.lo = 0x080fffe0}}, // 0x80000-0x9FFFF
       {MSR_GLIU0_SHADOW,  {.hi = 0x2000FFFF,.lo = 0xFFFF0003}}, // 0xC0000-0xFFFFF
       {MSR_GLIU1_BASE1,   {.hi = 0x20000000,.lo = 0x000fff80}}, // 0x00000-0x7FFFF
       {MSR_GLIU1_BASE2,   {.hi = 0x20000000,.lo = 0x080fffe0}}, // 0x80000-0x9FFFF
       {MSR_GLIU1_SHADOW,  {.hi = 0x2000FFFF,.lo = 0xFFFF0003}}, // 0xC0000-0xFFFFF
};


static void msr_init(void)
{
  int i;
       for (i = 0; i < ARRAY_SIZE(msr_table); i++)
                           wrmsr(msr_table[i].msrnum, msr_table[i].msr);
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

	/* Check memory. */
	/* ram_check(0x00000000, 640 * 1024); */

	/* Memory is setup. Return to cache_as_ram.inc and continue to boot. */
	return;
}
