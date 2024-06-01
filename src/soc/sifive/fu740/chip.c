/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <device/device.h>
#include <soc/addressmap.h>
#include <symbols.h>

static void fu740_init(struct device *dev)
{
	int index = 0;
	ram_from_to(dev, index++, FU740_DRAM, cbmem_top());
}

struct chip_operations soc_sifive_fu740_ops = {
	.name = "SIFIVE FU740",
	.enable_dev = fu740_init,
};
