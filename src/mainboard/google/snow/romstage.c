/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#include <types.h>
#include <system.h>

#include <armv7.h>
#include <cache.h>
#include <cbfs.h>
#include <common.h>

#include <arch/gpio.h>
#include <cpu/samsung/exynos5250/clk.h>
#include <cpu/samsung/exynos5250/dmc.h>
#include <cpu/samsung/exynos5250/gpio.h>
#include <cpu/samsung/exynos5250/setup.h>
#include <cpu/samsung/exynos5250/periph.h>
#include <cpu/samsung/exynos5250/power.h>
#include <cpu/samsung/exynos5250/clock_init.h>
#include <console/console.h>
#include <arch/stages.h>

#include "mainboard.h"

#define MMC0_GPIO_PIN	(58)

#if 0
static int board_wakeup_permitted(void)
{
	const int gpio = GPIO_Y10;
	int is_bad_wake;

	/* We're a bad wakeup if the gpio was defined and was high */
	is_bad_wake = ((gpio != -1) && gpio_get_value(gpio));

	return !is_bad_wake;
}
#endif

static void initialize_s5p_mshc(void)
{
	/* MMC0: Fixed, 8 bit mode, connected with GPIO. */
	if (clock_set_mshci(PERIPH_ID_SDMMC0))
		printk(BIOS_CRIT, "Failed to set clock for SDMMC0.\n");
	if (gpio_direction_output(MMC0_GPIO_PIN, 1)) {
		printk(BIOS_CRIT, "Unable to power on SDMMC0.\n");
	}
	gpio_set_pull(MMC0_GPIO_PIN, EXYNOS_GPIO_PULL_NONE);
	gpio_set_drv(MMC0_GPIO_PIN, EXYNOS_GPIO_DRV_4X);
	exynos_pinmux_config(PERIPH_ID_SDMMC0, PINMUX_FLAG_8BIT_MODE);

	/* MMC2: Removable, 4 bit mode, no GPIO. */
	clock_set_mshci(PERIPH_ID_SDMMC2);
	exynos_pinmux_config(PERIPH_ID_SDMMC2, 0);
}

static void graphics(void)
{
	exynos_pinmux_config(PERIPH_ID_DPHPD, 0);
}

static inline void dsb(void)
{
	asm volatile ("dsb");
}

/* FIXME: replace isb() from system.h */
static inline void my_isb(void)
{
	asm volatile ("isb");
}

void main(void)
{
	struct mem_timings *mem;
	struct arm_clk_ratios *arm_ratios;
	int ret;
	void *entry;
	unsigned int cr;

	clock_set_rate(PERIPH_ID_SPI1, 50000000); /* set spi clock to 50Mhz */

	/*
	 * FIXME: Do necessary I2C init so low-level PMIC code doesn't need to.
	 * Also, we should only call power_init() on cold boot.
	 */
	power_init();

	/* Clock must be initialized before console_init, otherwise you may need
	 * to re-initialize serial console drivers again. */
	mem = get_mem_timings();
	arm_ratios = get_arm_clk_ratios();
	system_clock_init(mem, arm_ratios);

	console_init();

	if (!mem) {
		printk(BIOS_CRIT, "Unable to auto-detect memory timings\n");
		while(1);
	}
	printk(BIOS_SPEW, "man: 0x%x type: 0x%x, div: 0x%x, mhz: 0x%x\n",
		mem->mem_manuf,
		mem->mem_type,
		mem->mpll_mdiv,
		mem->frequency_mhz);

	ret = ddr3_mem_ctrl_init(mem, DMC_INTERLEAVE_SIZE);
	if (ret) {
		printk(BIOS_ERR, "Memory controller init failed, err: %x\n",
		ret);
		while(1);
	}

	/* Set up MMU and caches */
	mmu_setup(CONFIG_SYS_SDRAM_BASE, CONFIG_DRAM_SIZE_MB);
//	cr = get_cr();
//	cr |= CR_C | CR_I;
//	set_cr(cr);

	/* Enable D-side prefetch */
	cr = get_acr();
	cr |= (1 << 2);
	set_acr(cr);
	dsb();
	my_isb();

	initialize_s5p_mshc();

	graphics();

	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, "fallback/coreboot_ram");
	printk(BIOS_INFO, "entry is 0x%p, leaving romstage.\n", entry);

	stage_exit(entry);
}
