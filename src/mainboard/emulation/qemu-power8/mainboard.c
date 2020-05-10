/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <cbmem.h>

static void mainboard_enable(struct device *dev)
{

	if (!dev) {
		die("No dev0; die\n");
	}

	/* Where does RAM live? */
	ram_resource(dev, 0, 2048, 32768);
	cbmem_recovery(0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
