/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018-2019, The Linux Foundation.  All rights reserved.
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

#include <bootblock_common.h>
#include <device/device.h>
#include <device/mmio.h>
#include <gpio.h>
#include <timestamp.h>
#include <soc/usb.h>

static struct usb_board_data usb0_board_data = {
	.pll_bias_control_2 = 0x22,
	.imp_ctrl1 = 0x08,
	.port_tune1 = 0xc5,
};

static void setup_usb(void)
{
	setup_usb_host0(&usb0_board_data);
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
