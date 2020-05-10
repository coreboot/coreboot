/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/sdram.h>
#include <symbols.h>

static void mainboard_enable(struct device *dev)
{
	ram_resource(dev, 0, (uintptr_t)_dram/KiB, sdram_size_mb()*KiB);
}

struct chip_operations mainboard_ops = {
	.name = CONFIG_MAINBOARD_PART_NUMBER,
	.enable_dev = mainboard_enable,
};
