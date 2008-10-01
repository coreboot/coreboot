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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
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
#include <arch/romcc_io.h>
#include <arch/hlt.h>
#include <stdlib.h>
#include "pc80/serial.c"
#include "arch/i386/lib/console.c"
#include "ram/ramtest.c"
#include "cpu/x86/bist.h"
#include "cpu/x86/msr.h"
#include <cpu/amd/lxdef.h>
#include <cpu/amd/geode_post_code.h>
#include "southbridge/amd/cs5536/cs5536.h"
#include "spd_table.h"


#define POST_CODE(x) outb(x, 0x80)

#include "southbridge/amd/cs5536/cs5536_early_smbus.c"
#include "southbridge/amd/cs5536/cs5536_early_setup.c"

#define DIMM0 0xA0
#define DIMM1 0xA2


static int spd_read_byte(unsigned device, unsigned address)
{
	int i;

	if (device == DIMM0){
		for (i=0; i < (ARRAY_SIZE(spd_table)); i++){
			if (spd_table[i].address == address){
				return spd_table[i].data;
			}
		}
	}

	/* returns 0xFF on any failures */
	return 0xFF;
}

#define ManualConf 0		/* Do automatic strapped PLL config */
/* CPU and GLIU mult/div 500/266*/
#define PLLMSRhi 0x0000039C /* 33MHz PCI, 0x000003DD for 66MHz PCI */
/* Hold Count - how long we will sit in reset */
#define PLLMSRlo 0x00DE6000

#include "northbridge/amd/lx/raminit.h"
#include "northbridge/amd/lx/pll_reset.c"
#include "northbridge/amd/lx/raminit.c"
#include "sdram/generic_sdram.c"
#include "cpu/amd/model_lx/cpureginit.c"
#include "cpu/amd/model_lx/syspreinit.c"

static void msr_init(void)
{
	msr_t msr;
	/* Setup access to the cache for under 1MB. */
	msr.hi = 0x24fffc02;
	msr.lo = 0x1000A000;	/* 0-A0000 write back */
	wrmsr(CPU_RCONF_DEFAULT, msr);

	msr.hi = 0x0;		/* write back */
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
	/* Early mainboard specific GPIO setup */
}

static void cs5536_setup_onchipuart2(void)
{
	msr_t msr;

	/* GPIO4 - UART2_TX */
	/* Set: Output Enable  (0x4) */
	outl(GPIOL_4_SET, GPIO_IO_BASE + GPIOL_OUTPUT_ENABLE);
	/* Set: OUTAUX1 Select (0x10) */
	outl(GPIOL_4_SET, GPIO_IO_BASE + GPIOL_OUT_AUX1_SELECT);
	/* GPIO4 - UART2_RX */
	/* Set: Input Enable   (0x20) */
	outl(GPIOL_3_SET, GPIO_IO_BASE + GPIOL_INPUT_ENABLE);
	/* Set: INAUX1 Select  (0x34) */
	outl(GPIOL_3_SET, GPIO_IO_BASE + GPIOL_IN_AUX1_SELECT);

	/* Set: GPIO 3 + 3 Pull Up  (0x18) */
	outl(GPIOL_3_SET | GPIOL_4_SET, GPIO_IO_BASE + GPIOL_PULLUP_ENABLE);

	/* set address to 3F8 */
	msr = rdmsr(MDD_LEG_IO);
	msr.lo |= 0x7 << 20;
	wrmsr(MDD_LEG_IO, msr);

	/* Bit 1 = DEVEN (device enable)
	 * Bit 4 = EN_BANKS (allow access to the upper banks
	 */
	msr.lo = (1 << 4) | (1 << 1);
	msr.hi = 0;

	/* enable COM2 */
	wrmsr(MDD_UART2_CONF, msr);
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

	/* NOTE: must do this AFTER the early_setup!
	 * it is counting on some early MSR setup
	 * for cs5536
	 */
	/* cs5536_disable_internal_uart	 disable them. Set them up now... */
	cs5536_setup_onchipuart2(); /* dbe61 uses UART2 as COM1 */
	mb_gpio_init();
	uart_init();
	console_init();

	pll_reset(ManualConf);

	cpuRegInit();

	sdram_initialize(1, memctrl);

	/* Dump memory configuratation */
	/*{
	msr_t msr;
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
	}*/

	/* Check memory. */
	/* ram_check(0x00000000, 640 * 1024); */

	/* Memory is setup. Return to cache_as_ram.inc and continue to boot */
	return;
}
