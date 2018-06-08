/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <device/smbus.h>
#include "chip.h"

#define SMBUS_BLOCK_SIZE 32

static void ck505_init(struct device *dev)
{
	struct drivers_i2c_ck505_config *config;
	int dev_nregs, nregs;
	u8 block[SMBUS_BLOCK_SIZE];
	int i;

	if (!dev->enabled || dev->path.type != DEVICE_PATH_I2C)
		return;

	config = dev->chip_info;

	dev_nregs = smbus_block_read(dev, 0, sizeof(block), block);

	if (dev_nregs < 0) {
		printk(BIOS_ERR, "Failed reading ck505 configuration!\n");
		return;
	}

	/* This means that the devicetree doesn't have to specify nregs */
	nregs = MIN(MIN(dev_nregs, config->nregs == 0 ? SMBUS_BLOCK_SIZE
				: config->nregs), ARRAY_SIZE(config->mask));


	printk(BIOS_DEBUG, "Changing %d of the %d ck505 config bytes.\n",
		nregs, dev_nregs);

	assert(ARRAY_SIZE(config->mask) == ARRAY_SIZE(config->regs));

	for (i = 0; i < nregs && i < SMBUS_BLOCK_SIZE; i++)
		block[i] = (block[i] & ~config->mask[i]) | config->regs[i];

	if (smbus_block_write(dev, 0, dev_nregs, block) < 0)
		printk(BIOS_ERR, "Failed writing ck505 configuration!\n");
}

static struct device_operations ck505_operations = {
	.read_resources		= DEVICE_NOOP,
	.set_resources		= DEVICE_NOOP,
	.enable_resources	= DEVICE_NOOP,
	.init			= ck505_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &ck505_operations;
}

struct chip_operations drivers_i2c_ck505_ops = {
	CHIP_NAME("CK505 Clock generator")
	.enable_dev = enable_dev,
};
