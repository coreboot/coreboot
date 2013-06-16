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
	exynos_pinmux_uart(GPIO_A04, 4);
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
		for (i = start_ext; i <= (start_ext + 3); i++) {
			gpio_cfg_pin(i, GPIO_FUNC(0x2));
			gpio_set_pull(i, GPIO_PULL_UP);
			gpio_set_drv(i, GPIO_DRV_4X);
		}
	}

	for (i = 0; i < 3; i++) {
		gpio_cfg_pin(start + i, GPIO_FUNC(0x2));
		gpio_set_pull(start + i, GPIO_PULL_NONE);
		gpio_set_drv(start + i, GPIO_DRV_4X);
	}

	for (i = 3; i <= 6; i++) {
		gpio_cfg_pin(start +  i, GPIO_FUNC(0x2));
		gpio_set_pull(start + i, GPIO_PULL_UP);
		gpio_set_drv(start + i, GPIO_DRV_4X);
	}
}

void exynos_pinmux_sdmmc0(void)
{
	gpio_set_pull(GPIO_C02, GPIO_PULL_UP);
	exynos_pinmux_sdmmc(GPIO_C00, GPIO_C30);
}

void exynos_pinmux_sdmmc1(void)
{
	exynos_pinmux_sdmmc(GPIO_C10, GPIO_D14);
}

void exynos_pinmux_sdmmc2(void)
{
	exynos_pinmux_sdmmc(GPIO_C20, 0);
}

static void exynos_pinmux_spi(int start, int cfg)
{
	int i;

	for (i = start; i < start + 4; i++)
		gpio_cfg_pin(i, cfg);
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
	exynos_pinmux_spi(GPIO_F10, 0x2);
}

void exynos_pinmux_spi4(void)
{
	int i;

	for (i = 0; i < 2; i++) {
		gpio_cfg_pin(GPIO_F02 + i, GPIO_FUNC(0x4));
		gpio_cfg_pin(GPIO_E04 + i, GPIO_FUNC(0x4));
	}
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

void exynos_pinmux_i2c8(void)
{
	exynos_pinmux_i2c(GPIO_B34, 0x2);
}

void exynos_pinmux_i2c9(void)
{
	exynos_pinmux_i2c(GPIO_B36, 0x2);
}

void exynos_pinmux_i2c10(void)
{
	exynos_pinmux_i2c(GPIO_B40, 0x2);
}

void exynos_pinmux_dphpd(void)
{
	gpio_cfg_pin(GPIO_X07, GPIO_FUNC(0x3));
	gpio_set_pull(GPIO_X07, GPIO_PULL_NONE);
}
