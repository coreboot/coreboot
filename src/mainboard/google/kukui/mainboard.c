/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#include <device/device.h>
#include <soc/gpio.h>
#include <soc/mmu_operations.h>
#include <soc/mtcmos.h>
#include <soc/usb.h>

static void configure_emmc(void)
{
	const gpio_t emmc_pin[] = {
		GPIO(MSDC0_DAT0), GPIO(MSDC0_DAT1),
		GPIO(MSDC0_DAT2), GPIO(MSDC0_DAT3),
		GPIO(MSDC0_DAT4), GPIO(MSDC0_DAT5),
		GPIO(MSDC0_DAT6), GPIO(MSDC0_DAT7),
		GPIO(MSDC0_CMD), GPIO(MSDC0_RSTB),
	};

	for (size_t i = 0; i < ARRAY_SIZE(emmc_pin); i++)
		gpio_set_pull(emmc_pin[i], GPIO_PULL_ENABLE, GPIO_PULL_UP);
}

static void configure_usb(void)
{
	setup_usb_host();
}

static void configure_audio(void)
{
	/* Audio PWR*/
	mtcmos_audio_power_on();

	/* SoC I2S */
	gpio_set_mode(GPIO(CAM_RST0), PAD_CAM_RST0_FUNC_I2S2_LRCK);
	gpio_set_mode(GPIO(CAM_PDN1), PAD_CAM_PDN1_FUNC_I2S2_BCK);
	gpio_set_mode(GPIO(CAM_PDN0), PAD_CAM_PDN0_FUNC_I2S2_MCK);
	gpio_set_mode(GPIO(EINT3), PAD_EINT3_FUNC_I2S3_DO);
}
static void mainboard_init(struct device *dev)
{
	configure_emmc();
	configure_usb();
	configure_audio();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
