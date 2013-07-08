/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
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

#include <console/console.h>
#include <assert.h>
#include "gpio.h"
#include "cpu.h"
#include "pinmux.h"

static void exynos_pinmux_uart(int start, int count)
{
	int i;

	for (i = start; i < start + count; i++) {
		gpio_set_pull(i, GPIO_PULL_NONE);
		gpio_cfg_pin(i, GPIO_FUNC(0x2));
	}
}

void exynos_pinmux_uart0(void)
{
	exynos_pinmux_uart(GPIO_A00, 4);
}

void exynos_pinmux_uart1(void)
{
	exynos_pinmux_uart(GPIO_D00, 4);
}

void exynos_pinmux_uart2(void)
{
	exynos_pinmux_uart(GPIO_A10, 4);
}

void exynos_pinmux_uart3(void)
{
	exynos_pinmux_uart(GPIO_A14, 2);
}

static void exynos_pinmux_sdmmc(int start, int start_ext)
{
	int i;

	if (start_ext) {
		for (i = 0; i <= 3; i++) {
			gpio_cfg_pin(start_ext + i, GPIO_FUNC(0x2));
			gpio_set_pull(start_ext + i, GPIO_PULL_UP);
			gpio_set_drv(start_ext + i, GPIO_DRV_4X);
		}
	}
	for (i = 0; i < 2; i++) {
		gpio_cfg_pin(start + i, GPIO_FUNC(0x2));
		gpio_set_pull(start + i, GPIO_PULL_NONE);
		gpio_set_drv(start + i, GPIO_DRV_4X);
	}
	for (i = 2; i <= 6; i++) {
		gpio_cfg_pin(start + i, GPIO_FUNC(0x2));
		gpio_set_pull(start + i, GPIO_PULL_UP);
		gpio_set_drv(start + i, GPIO_DRV_4X);
	}
}

void exynos_pinmux_sdmmc0(void)
{
	exynos_pinmux_sdmmc(GPIO_C00, GPIO_C10);
}

void exynos_pinmux_sdmmc1(void)
{
	exynos_pinmux_sdmmc(GPIO_C20, 0);
}

void exynos_pinmux_sdmmc2(void)
{
	exynos_pinmux_sdmmc(GPIO_C30, 0);
}

void exynos_pinmux_sdmmc3(void)
{
	/*
	 * TODO: Need to add definitions for GPC4 before
	 * enabling this.
	 */
	printk(BIOS_DEBUG, "SDMMC3 not supported yet");
}

void exynos_pinmux_sromc(int bank, int sixteen_bit)
{
	int i;

	if (bank > 3) {
		printk(BIOS_DEBUG, "Unsupported sromc bank %d.\n", bank);
		return;
	}

	gpio_cfg_pin(GPIO_Y00 + bank, GPIO_FUNC(2));
	gpio_cfg_pin(GPIO_Y04, GPIO_FUNC(2));
	gpio_cfg_pin(GPIO_Y05, GPIO_FUNC(2));

	for (i = 2; i < 4; i++)
		gpio_cfg_pin(GPIO_Y10 + i, GPIO_FUNC(2));

	for (i = 0; i < 8; i++) {
		gpio_cfg_pin(GPIO_Y30 + i, GPIO_FUNC(2));
		gpio_set_pull(GPIO_Y30 + i, GPIO_PULL_UP);

		gpio_cfg_pin(GPIO_Y50 + i, GPIO_FUNC(2));
		gpio_set_pull(GPIO_Y50 + i, GPIO_PULL_UP);

		if (sixteen_bit) {
			gpio_cfg_pin(GPIO_Y60 + i, GPIO_FUNC(2));
			gpio_set_pull(GPIO_Y60 + i, GPIO_PULL_UP);
		}
	}
}

static void exynos_pinmux_spi(int start, int cfg)
{
	int i;

	for (i = 0; i < 4; i++)
		gpio_cfg_pin(start + i, GPIO_FUNC(cfg));
}

void exynos_pinmux_spi0(void)
{
	exynos_pinmux_spi(GPIO_A20, 0x2);
}

void exynos_pinmux_spi1(void)
{
	exynos_pinmux_spi(GPIO_A24, 0x2);
}

void exynos_pinmux_spi2(void)
{
	exynos_pinmux_spi(GPIO_B11, 0x5);
}

void exynos_pinmux_spi3(void)
{
	exynos_pinmux_spi(GPIO_E00, 0x2);
}

void exynos_pinmux_spi4(void)
{
	int i;

	for (i = 0; i < 2; i++)
		gpio_cfg_pin(GPIO_F02 + i, GPIO_FUNC(0x4));
	for (i = 2; i < 4; i++)
		gpio_cfg_pin(GPIO_E02 + i, GPIO_FUNC(0x4));
}

void exynos_pinmux_backlight(void)
{
	gpio_cfg_pin(GPIO_B20, GPIO_OUTPUT);
	gpio_set_value(GPIO_B20, 1);
}

void exynos_pinmux_lcd(void)
{
	gpio_cfg_pin(GPIO_Y25, GPIO_OUTPUT);
	gpio_set_value(GPIO_Y25, 1);
	gpio_cfg_pin(GPIO_X15, GPIO_OUTPUT);
	gpio_set_value(GPIO_X15, 1);
	gpio_cfg_pin(GPIO_X30, GPIO_OUTPUT);
	gpio_set_value(GPIO_X30, 1);
}

static void exynos_pinmux_i2c(int start, int func)
{
	gpio_cfg_pin(start, GPIO_FUNC(func));
	gpio_cfg_pin(start + 1, GPIO_FUNC(func));
	gpio_set_pull(start, GPIO_PULL_NONE);
	gpio_set_pull(start + 1, GPIO_PULL_NONE);
}

void exynos_pinmux_i2c0(void)
{
	exynos_pinmux_i2c(GPIO_B30, 0x2);
}

void exynos_pinmux_i2c1(void)
{
	exynos_pinmux_i2c(GPIO_B32, 0x2);
}

void exynos_pinmux_i2c2(void)
{
	exynos_pinmux_i2c(GPIO_A06, 0x3);
}

void exynos_pinmux_i2c3(void)
{
	exynos_pinmux_i2c(GPIO_A12, 0x3);
}

void exynos_pinmux_i2c4(void)
{
	exynos_pinmux_i2c(GPIO_A20, 0x3);
}

void exynos_pinmux_i2c5(void)
{
	exynos_pinmux_i2c(GPIO_A22, 0x3);
}

void exynos_pinmux_i2c6(void)
{
	exynos_pinmux_i2c(GPIO_B13, 0x4);
}

void exynos_pinmux_i2c7(void)
{
	exynos_pinmux_i2c(GPIO_B22, 0x3);
}

void exynos_pinmux_dphpd(void)
{
	/* Set Hotplug detect for DP */
	gpio_cfg_pin(GPIO_X07, GPIO_FUNC(0x3));

	/*
	 * Hotplug detect should have an external pullup; disable the
	 * internal pulldown so they don't fight.
	 */
	gpio_set_pull(GPIO_X07, GPIO_PULL_NONE);
}

void exynos_pinmux_i2s1(void)
{
	int i;

	for (i = 0; i < 5; i++) {
		gpio_cfg_pin(GPIO_B00 + i, GPIO_FUNC(0x02));
		gpio_set_pull(GPIO_B00 + i, GPIO_PULL_NONE);
	}
}
