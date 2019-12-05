/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2016 Intel Corporation.
 * Copyright (C) 2018-2019 Eltan B.V.
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

/*
 * Declare the resources we are using
 */
static void mainboard_reserve_resources(struct device *dev)
{
	unsigned int idx = 0;
	struct resource *res;

	/*
	 * CPLD: Reserve the IRQ here all others are within the default LPC
	 * range 0 to 1000h
	 */
	res = new_resource(dev, idx++);
	res->base = 0x7;
	res->size = 0x1;
	res->flags = IORESOURCE_IRQ | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

/*
 * mainboard_enable is executed as first thing after
 * enumerate_buses().
 */
static void mainboard_enable(struct device *dev)
{
	mainboard_reserve_resources(dev);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
