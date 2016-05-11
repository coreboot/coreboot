/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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
 */

#include <boardid.h>
#include <device/device.h>
#include <gpio.h>
#include <soc/clock.h>
#include <soc/grf.h>

static void configure_sdmmc(void)
{
	gpio_output(GPIO(4, D, 5), 1);  /* SDMMC_PWR_EN */
	gpio_output(GPIO(2, A, 2), 1);  /* SDMMC_SDIO_PWR_EN */

	/* SDMMC_DET_L is different on Kevin board revision 0. */
	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_KEVIN) && (board_id() == 0))
		gpio_input(GPIO(4, D, 2));
	else
		gpio_input(GPIO(4, D, 0));

	gpio_output(GPIO(2, D, 4), 0);  /* Keep the max voltage */

	write32(&rk3399_grf->iomux_sdmmc, IOMUX_SDMMC);
}

static void mainboard_init(device_t dev)
{
	configure_sdmmc();
}

static void mainboard_enable(device_t dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
