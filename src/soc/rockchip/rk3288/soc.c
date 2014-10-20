/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

#include <arch/cache.h>
#include <cbmem.h>
#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <soc/gpio.h>
#include <soc/soc.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "chip.h"

static void soc_enable(device_t dev)
{
	ram_resource(dev, 0, RAM_BASE_KB, RAM_SIZE_KB);
}

static void soc_init(device_t dev)
{

}

static void soc_noop(device_t dev)
{

}

static struct device_operations soc_ops = {
	.read_resources   = soc_noop,
	.set_resources    = soc_noop,
	.enable_resources = soc_enable,
	.init             = soc_init,
	.scan_bus         = 0,
};

static void enable_rk3288_dev(device_t dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_rockchip_rk3288_ops = {
	CHIP_NAME("SOC Rockchip 3288")
	.enable_dev = enable_rk3288_dev,
};
