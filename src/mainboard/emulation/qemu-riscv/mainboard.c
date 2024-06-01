/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <symbols.h>
#include <cbmem.h>

static void mainboard_enable(struct device *dev)
{
	if (!dev) {
		die("No dev0; die\n");
	}

	ram_from_to(dev, 0, (uintptr_t)_dram, cbmem_top());
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
