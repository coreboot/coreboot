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

#include <armv7.h>
#include <cbfs.h>
#include <common.h>

#include <arch/cache.h>
#include <arch/gpio.h>
#include <cpu/samsung/exynos5-common/i2c.h>
#include <cpu/samsung/exynos5250/clk.h>
#include <cpu/samsung/exynos5250/cpu.h>
#include <cpu/samsung/exynos5250/dmc.h>
#include <cpu/samsung/exynos5250/gpio.h>
#include <cpu/samsung/exynos5250/setup.h>
#include <cpu/samsung/exynos5250/periph.h>
#include <cpu/samsung/exynos5250/power.h>
#include <cpu/samsung/exynos5250/clock_init.h>
#include <console/console.h>
#include <arch/stages.h>
#include <time.h>

#include <drivers/maxim/max77686/max77686.h>
#include <device/i2c.h>

#include "mainboard.h"

#define PMIC_BUS	0
#define MMC0_GPIO_PIN	(58)

static void snow_setup_power(void)
{
	int error = 0;

	power_init();

	/* Initialize I2C bus to configure PMIC. */
	i2c_init(0, CONFIG_SYS_I2C_SPEED, 0x00);

	printk(BIOS_DEBUG, "%s: Setting up PMIC...\n", __func__);
	/*
	 * We're using CR1616 coin cell battery that is non-rechargeable
	 * battery. But, BBCHOSTEN bit of the BBAT Charger Register in
	 * MAX77686 is enabled by default for charging coin cell.
	 *
	 * Also, we cannot meet the coin cell reverse current spec. in UL
	 * standard if BBCHOSTEN bit is enabled.
	 *
	 * Disable Coin BATT Charging
	 */
	error = max77686_disable_backup_batt(PMIC_BUS);

	error |= max77686_volsetting(PMIC_BUS, PMIC_BUCK2, CONFIG_VDD_ARM_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_BUS, PMIC_BUCK3, CONFIG_VDD_INT_UV,
						REG_ENABLE, MAX77686_UV);
	error |= max77686_volsetting(PMIC_BUS, PMIC_BUCK1, CONFIG_VDD_MIF_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_BUS, PMIC_BUCK4, CONFIG_VDD_G3D_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_BUS, PMIC_LDO2, CONFIG_VDD_LDO2_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_BUS, PMIC_LDO3, CONFIG_VDD_LDO3_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_BUS, PMIC_LDO5, CONFIG_VDD_LDO5_MV,
						REG_ENABLE, MAX77686_MV);
	error |= max77686_volsetting(PMIC_BUS, PMIC_LDO10, CONFIG_VDD_LDO10_MV,
						REG_ENABLE, MAX77686_MV);

	error |= max77686_enable_32khz_cp(PMIC_BUS);

	if (error) {
		printk(BIOS_CRIT, "%s: PMIC error: %#x\n", __func__, error);
		die("Failed to intialize PMIC.\n");
	}
}

static void snow_setup_storage(void)
{
	/* MMC0: Fixed, 8 bit mode, connected with GPIO. */
	if (clock_set_mshci(PERIPH_ID_SDMMC0))
		printk(BIOS_CRIT, "%s: Failed to set MMC0 clock.\n", __func__);
	if (gpio_direction_output(MMC0_GPIO_PIN, 1)) {
		printk(BIOS_CRIT, "%s: Unable to power on MMC0.\n", __func__);
	}
	gpio_set_pull(MMC0_GPIO_PIN, EXYNOS_GPIO_PULL_NONE);
	gpio_set_drv(MMC0_GPIO_PIN, EXYNOS_GPIO_DRV_4X);
	exynos_pinmux_config(PERIPH_ID_SDMMC0, PINMUX_FLAG_8BIT_MODE);

	/* MMC2: Removable, 4 bit mode, no GPIO. */
	clock_set_mshci(PERIPH_ID_SDMMC2);
	exynos_pinmux_config(PERIPH_ID_SDMMC2, 0);
}

static void snow_setup_graphics(void)
{
	exynos_pinmux_config(PERIPH_ID_DPHPD, 0);
}

static void snow_setup_gpio(void)
{
	struct exynos5_gpio_part1 *gpio_pt1;
	struct exynos5_gpio_part2 *gpio_pt2;

	enum {
		WP_GPIO = 6,
		RECMODE_GPIO = 0,
		LID_GPIO = 5,
		POWER_GPIO = 3
	};

	gpio_pt1 = (struct exynos5_gpio_part1 *)EXYNOS5_GPIO_PART1_BASE;
	gpio_pt2 = (struct exynos5_gpio_part2 *)EXYNOS5_GPIO_PART2_BASE;

	s5p_gpio_direction_input(&gpio_pt1->d1, WP_GPIO);
	s5p_gpio_set_pull(&gpio_pt1->d1, WP_GPIO, EXYNOS_GPIO_PULL_NONE);

	s5p_gpio_direction_input(&gpio_pt1->y1, RECMODE_GPIO);
	s5p_gpio_set_pull(&gpio_pt1->y1, RECMODE_GPIO, EXYNOS_GPIO_PULL_NONE);

	s5p_gpio_direction_input(&gpio_pt2->x3, LID_GPIO);
	s5p_gpio_set_pull(&gpio_pt2->x3, LID_GPIO, EXYNOS_GPIO_PULL_NONE);

	s5p_gpio_direction_input(&gpio_pt2->x1, POWER_GPIO);
	s5p_gpio_set_pull(&gpio_pt2->x1, POWER_GPIO, EXYNOS_GPIO_PULL_NONE);
}

static void snow_setup_memory(struct mem_timings *mem, int is_resume)
{
	printk(BIOS_SPEW, "man: 0x%x type: 0x%x, div: 0x%x, mhz: 0x%x\n",
	       mem->mem_manuf,
	       mem->mem_type,
	       mem->mpll_mdiv,
	       mem->frequency_mhz);

	/* FIXME Currently memory initialization with mem_reset on normal boot
	 * will cause resume to fail (even if we don't do mem_reset on resume),
	 * and the workaround is to temporarily always enable "is_resume".
	 * This should be removed when the root cause of resume issue is found.
	 */
	is_resume = 1;

	if (ddr3_mem_ctrl_init(mem, DMC_INTERLEAVE_SIZE, !is_resume)) {
		die("Failed to initialize memory controller.\n");
	}
}

static struct mem_timings *snow_setup_clock(void)
{
	struct mem_timings *mem = get_mem_timings();
	struct arm_clk_ratios *arm_ratios = get_arm_clk_ratios();
	if (!mem) {
		die("Unable to auto-detect memory timings\n");
	}
	system_clock_init(mem, arm_ratios);
	return mem;
}

void main(void)
{
	struct mem_timings *mem;
	void *entry;
	int is_resume = (snow_get_wakeup_state() != SNOW_IS_NOT_WAKEUP);

	/* Clock must be initialized before console_init, otherwise you may need
	 * to re-initialize serial console drivers again. */
	mem = snow_setup_clock();

	if (!is_resume) {
		console_init();
		snow_setup_power();
	}

	snow_setup_memory(mem, is_resume);

	if (is_resume) {
		snow_wakeup();
	}

	snow_setup_storage();
	snow_setup_gpio();
	snow_setup_graphics();

	/* Set SPI (primary CBFS media) clock to 50MHz. */
	clock_set_rate(PERIPH_ID_SPI1, 50000000);
	entry = cbfs_load_stage(CBFS_DEFAULT_MEDIA, "fallback/coreboot_ram");
	printk(BIOS_INFO, "entry is 0x%p, leaving romstage.\n", entry);

	stage_exit(entry);
}
