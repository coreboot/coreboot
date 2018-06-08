/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdint.h>
#include <stdlib.h>
#include <spd.h>
#include <device/pci_def.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <console/console.h>
#include <lib.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/lxdef.h>
#include <cpu/amd/car.h>
#include <southbridge/amd/cs5536/cs5536.h>
#include <northbridge/amd/lx/raminit.h>
#include <northbridge/amd/lx/northbridge.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627HF_SP1)

/* The part is a Hynix hy5du121622ctp-d43.
 *
 * HY 5D U 12 16 2 2 C <blank> T <blank> P D43
 * Hynix
 * DDR SDRAM (5D)
 * VDD 2.5 VDDQ 2.5 (U)
 * 512M 8K REFRESH (12)
 * x16 (16)
 * 4banks (2)
 * SSTL_2 (2)
 * 4th GEN die (C)
 * Normal Power Consumption (<blank> )
 * TSOP (T)
 * Single Die (<blank>)
 * Lead Free (P)
 * DDR400 3-3-3 (D43)
 */
/* SPD array */
static const u8 spdbytes[] = {
	[SPD_ACCEPTABLE_CAS_LATENCIES] = 0x10,
	[SPD_BANK_DENSITY] = 0x40,
	[SPD_DEVICE_ATTRIBUTES_GENERAL] = 0xff,
	[SPD_MEMORY_TYPE] = 7,
	[SPD_MIN_CYCLE_TIME_AT_CAS_MAX] = 10, /* A guess for the tRAC value */
	[SPD_MODULE_ATTRIBUTES] = 0xff, /* FIXME later when we figure out. */
	[SPD_NUM_BANKS_PER_SDRAM] = 4,
	[SPD_PRIMARY_SDRAM_WIDTH] = 8,
	[SPD_NUM_DIMM_BANKS] = 1, /* ALIX1.C is 1 bank. */
	[SPD_NUM_COLUMNS] = 0xa,
	[SPD_NUM_ROWS] = 3,
	[SPD_REFRESH] = 0x3a,
	[SPD_SDRAM_CYCLE_TIME_2ND] = 60,
	[SPD_SDRAM_CYCLE_TIME_3RD] = 75,
	[SPD_tRAS] = 40,
	[SPD_tRCD] = 15,
	[SPD_tRFC] = 70,
	[SPD_tRP] = 15,
	[SPD_tRRD] = 10,
};

int spd_read_byte(unsigned int device, unsigned int address)
{
	printk(BIOS_DEBUG, "spd_read_byte dev %02x", device);

	if (device != DIMM0) {
		printk(BIOS_DEBUG, " returns 0xff\n");
		return 0xff;
	}

	printk(BIOS_DEBUG, " addr %02x returns %02x\n",
		address, spdbytes[address]);

	return spdbytes[address];
}

/** Early mainboard specific GPIO setup. */
static void mb_gpio_init(void)
{
	/*
	 * Enable LEDs GPIO outputs to light up the leds
	 * This is how the original tinyBIOS sets them after boot.
	 * Info: GPIO_IO_BASE, 0x6100, is only valid before PCI init, so it
	 *       may be used here, but not after PCI Init.
	 * Note: Prior to a certain release, Linux used a hardwired 0x6100 in the
	 *       leds-alix2.c driver. coreboot dynamically assigns this space,
	 *       so the driver does not work anymore.
	 *       Good workaround: use the newer driver
	 *       Ugly workaround: $ wrmsr 0x5140000C 0xf00100006100
	 *         This resets the GPIO I/O space to 0x6100.
	 *         This may break other things, though.
	 */
	outl(1 << 6, GPIO_IO_BASE + GPIOL_OUTPUT_ENABLE);
	outl(1 << 9, GPIO_IO_BASE + GPIOH_OUTPUT_ENABLE);
	outl(1 << 11, GPIO_IO_BASE + GPIOH_OUTPUT_ENABLE);

	/* outl(1 << 6, GPIO_IO_BASE + GPIOL_OUTPUT_VALUE); */  /* Led 1 enabled  */
	outl(1 << 9, GPIO_IO_BASE + GPIOH_OUTPUT_VALUE);        /* Led 2 disabled */
	outl(1 << 11, GPIO_IO_BASE + GPIOH_OUTPUT_VALUE);       /* Led 3 disabled */
}

void asmlinkage mainboard_romstage_entry(unsigned long bist)
{
	static const struct mem_controller memctrl[] = {
		{.channel0 = {DIMM0}},
	};

	SystemPreInit();
	lx_msr_init();

	cs5536_early_setup();

	/* NOTE: Must do this AFTER cs5536_early_setup()!
	 * It is counting on some early MSR setup for the CS5536.
	 */
	cs5536_setup_onchipuart(1);
	mb_gpio_init();
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	lx_pll_reset();

	cpuRegInit(0, DIMM0, DIMM1, DRAM_TERMINATED);

	sdram_initialize(1, memctrl);

	/* Switch from Cache as RAM to real RAM.
	 *
	 * There are two ways we could think about this.
	 *
	 * 1. If we are using the romstage.inc ROMCC way, the stack is
	 * going to be re-setup in the code following this code.  Just
	 * wbinvd the stack to clear the cache tags.  We don't care
	 * where the stack used to be.
	 *
	 * 2. This file is built as a normal .c -> .o and linked in
	 * etc.  The stack might be used to return etc.  That means we
	 * care about what is in the stack.  If we are smart we set
	 * the CAR stack to the same location as the rest of
	 * coreboot. If that is the case we can just do a wbinvd.
	 * The stack will be written into real RAM that is now setup
	 * and we continue like nothing happened.  If the stack is
	 * located somewhere other than where LB would like it, you
	 * need to write some code to do a copy from cache to RAM
	 *
	 * We use method 1 on Norwich and on this board too.
	 */
	post_code(0x02);
	printk(BIOS_ERR, "POST 02\n");
	__asm__("wbinvd\n");
	printk(BIOS_ERR, "Past wbinvd\n");

	/* We are finding the return does not work on this board. Explicitly
	 * call the label that is after the call to us. This is gross, but
	 * sometimes at this level it is the only way out.
	 */
	done_cache_as_ram_main();
}
