/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <cbmem.h>

static void mainboard_enable(struct device *dev)
{
	if (!dev)
		die("No dev0; die\n");
	ram_resource_kb(dev, 0, 0, (unsigned long)cbmem_top() / KiB);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
