/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/device.h>
#include <gpio.h>
#include <soc/usb.h>

static struct usb_board_data usb1_board_data = {
	.pll_bias_control_2 = 0x28,
	.imp_ctrl1 = 0x08,
	.port_tune1 = 0x20,
};

static void setup_usb(void)
{
	/*
	 * Primary USB is used only for DP functionality on cheza platform.
	 * Hence Setting up only Secondary USB DWC3 controller.
	 */
	setup_usb_host1(&usb1_board_data);

	gpio_output(GPIO(120), 1);	/* Deassert HUB_RST_L to enable hub. */
}

static void mainboard_init(struct device *dev)
{
	setup_usb();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = &mainboard_init;
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
