/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/sdram.h>
#include <symbols.h>

static void mainboard_enable(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)_dram, sdram_size_mb());
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
