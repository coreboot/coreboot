/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <device/device.h>

static void enable_soc_dev(struct device *dev)
{
	ram_range(dev, 0, 0, cbmem_top_chipset());
	/* This is for OCC and HOMER images */
	reserved_ram_range(dev, 1, cbmem_top_chipset(), 256 * MiB);
}

struct chip_operations soc_ibm_power9_ops = {
	.name = "POWER9",
	.enable_dev = enable_soc_dev,
};
