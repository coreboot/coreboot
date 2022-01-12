/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include "chip.h"
#include <arch/ioapic.h>

static void ioapic_init(struct device *dev)
{
	struct drivers_generic_ioapic_config *config = dev->chip_info;

	if (!dev->enabled || !config)
		return;

	setup_ioapic(config->base, config->apicid);
}

static void ioapic_read_resources(struct device *dev)
{
	struct drivers_generic_ioapic_config *config = (struct drivers_generic_ioapic_config *)dev->chip_info;
	struct resource *res;

	res = new_resource(dev, 0);
	res->base = (resource_t)(uintptr_t)config->base;
	res->size = 0x1000;
	res->flags = IORESOURCE_MEM | IORESOURCE_ASSIGNED | IORESOURCE_FIXED;
}

static struct device_operations ioapic_operations = {
	.read_resources   = ioapic_read_resources,
	.set_resources    = noop_set_resources,
	.init             = ioapic_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &ioapic_operations;
}

struct chip_operations drivers_generic_ioapic_ops = {
	CHIP_NAME("IOAPIC")
	.enable_dev = enable_dev,
};
