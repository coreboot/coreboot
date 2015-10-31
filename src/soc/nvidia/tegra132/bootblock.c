/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <arch/exception.h>
#include <arch/hlt.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/nvidia/tegra/apbmisc.h>
#include <soc/power.h>
#include <timestamp.h>

#define BCT_OFFSET_IN_BIT	0x50
#define ODMDATA_OFFSET_IN_BCT	0x6A8
#define TEGRA_SRAM_MAX		(TEGRA_SRAM_BASE + TEGRA_SRAM_SIZE)

static void save_odmdata(void)
{
	struct tegra_pmc_regs *pmc = (struct tegra_pmc_regs*)TEGRA_PMC_BASE;
	uintptr_t bct_offset;
	u32 odmdata;

	// pmc.odmdata: [18:19]: console type, [15:17]: UART id.
	// TODO(twarren) ODMDATA is stored in the BCT, from bct/odmdata.cfg.
	// I use the BCT offset in the BIT in SRAM to locate the BCT, and
	// pick up the ODMDATA word at BCT offset 0x6A8. I could use a BCT
	// struct header from cbootimage, but it seems like overkill for this.

	bct_offset = read32((void *)(TEGRA_SRAM_BASE + BCT_OFFSET_IN_BIT));
	if (bct_offset > TEGRA_SRAM_BASE && bct_offset < TEGRA_SRAM_MAX) {
		odmdata = read32((void *)(bct_offset + ODMDATA_OFFSET_IN_BCT));
		write32(&pmc->odmdata, odmdata);
	}
}

void __attribute__((weak)) bootblock_mainboard_early_init(void)
{
	/* Empty default implementation. */
}

void main(void)
{
	timestamp_init(0);
	timestamp_add_now(TS_START_BOOTBLOCK);

	// enable JTAG at the earliest stage
	enable_jtag();

	clock_early_uart();

	/* Configure mselect clock. */
	clock_configure_source(mselect, PLLP, 102000);

	/* Enable AVP cache, timer, APB dma, and mselect blocks.  */
	clock_enable_clear_reset(CLK_L_CACHE2 | CLK_L_TMR,
				 CLK_H_APBDMA,
				 0, CLK_V_MSELECT, 0, 0);

	/* Find ODMDATA in IRAM and save it to scratch reg */
	save_odmdata();

	bootblock_mainboard_early_init();

	if (CONFIG_BOOTBLOCK_CONSOLE) {
		console_init();
		exception_init();
		printk(BIOS_INFO, "T132: Bootblock here\n");
	}

	clock_init();

	printk(BIOS_INFO, "T132 bootblock: Clock init done\n");

	pmc_print_rst_status();

	bootblock_mainboard_init();

	printk(BIOS_INFO, "T132 bootblock: Mainboard bootblock init done\n");

	run_romstage();
}
