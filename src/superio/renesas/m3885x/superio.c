/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <device/device.h>
#include <device/pnp.h>
#include <console/console.h>
#include <device/smbus.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <pc80/keyboard.h>

void m3885_configure_multikey(void);

static void m3885x_init(struct device *dev)
{

	if (!dev->enabled)
		return;

	printk(BIOS_DEBUG, "Renesas M3885x: Initializing keyboard.\n");
	set_kbc_ps2_mode();
	pc_keyboard_init(NO_AUX_DEVICE);
	m3885_configure_multikey();
}

static void m3885x_read_resources(struct device *dev)
{
	/* Nothing, but this function avoids an error on serial console. */
}

static void m3885x_enable_resources(struct device *dev)
{
	/* Nothing, but this function avoids an error on serial console. */
}

static struct device_operations ops = {
	.init             = m3885x_init,
	.read_resources   = m3885x_read_resources,
	.enable_resources = m3885x_enable_resources
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, 0, 0, 0, }
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations superio_renesas_m3885x_ops = {
	CHIP_NAME("Renesas M3885x Super I/O")
	.enable_dev = enable_dev,
};
