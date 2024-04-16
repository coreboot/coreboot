/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_util.h>
#include <device/device.h>

/*
 * This controls the device -> IRQ routing.
 *
 */
static const struct fch_irq_routing fch_irq_map[] = {
};

const struct fch_irq_routing *mb_get_fch_irq_mapping(size_t *length)
{
	*length = ARRAY_SIZE(fch_irq_map);
	return fch_irq_map;
}

static void mainboard_init(void *chip_info)
{
}

static void mainboard_enable(struct device *dev)
{
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
