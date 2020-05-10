/* SPDX-License-Identifier: GPL-2.0-only */

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
