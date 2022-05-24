/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>

static void mainboard_enable(struct device *dev)
{
	/*
	 * Size of the emulated system RAM. On hardware, this would be external
	 * DDR memory.
	 *
	 * TODO: Get this size from the hardware-supplied configuration string.
	 */
	const size_t ram_size = 1*GiB;

	if (!dev) {
		die("No dev0; die\n");
	}

	ram_resource_kb(dev, 0, 0x80000000/KiB, ram_size/KiB);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
