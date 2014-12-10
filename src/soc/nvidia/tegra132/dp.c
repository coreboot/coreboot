/*
 * drivers/video/tegra/dc/dp.c
 *
 * Copyright (c) 2011-2013, NVIDIA Corporation.
 * Copyright 2014 Google Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/display.h>
#include <soc/nvidia/tegra/dc.h>
#include <stdlib.h>

#include "chip.h"

void dp_display_startup(device_t dev)
{
	struct soc_nvidia_tegra132_config *config = dev->chip_info;
	struct display_controller *disp_ctrl =
			(void *)config->display_controller;

	printk(BIOS_INFO, "%s: entry: disp_ctrl: %p.\n",
		 __func__, disp_ctrl);
}
