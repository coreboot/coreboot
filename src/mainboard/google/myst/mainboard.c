/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/amd_pci_util.h>
#include <baseboard/variants.h>
#include <device/device.h>

static const struct fch_irq_routing fch_irq_map[] = {
	{ 0,	0x00,		0x00 },
};

const struct fch_irq_routing *mb_get_fch_irq_mapping(size_t *length)
{
	*length = ARRAY_SIZE(fch_irq_map);
	return fch_irq_map;
}

static void mainboard_init(void *chip_info)
{
	/* TODO(b/270596581): Perform mainboard initialization */
}

static void mainboard_enable(struct device *dev)
{
	/* TODO(b/270618107): Enable mainboard */
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
