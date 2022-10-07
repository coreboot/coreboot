/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <console/console.h>
#include <stdint.h>
#include <gpio.h>
#include <arch/io.h>

#include "gpio.h"

static uint8_t get_hsid(void)
{
	const gpio_t hsid_gpios[] = {
		GPP_A8,
		GPP_F19,
		GPP_H23,
		GPP_H19,
	};
	return gpio_base2_value(hsid_gpios, ARRAY_SIZE(hsid_gpios));
}

static void mainboard_init(void *chip_info)
{
	configure_gpio_pads();
	printk(BIOS_INFO, "HSID: 0x%x\n", get_hsid());
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
