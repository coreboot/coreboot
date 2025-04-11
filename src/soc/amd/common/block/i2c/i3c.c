/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>

static void i3c_read_resources(struct device *dev)
{
	mmio_range(dev, 0, dev->path.mmio.addr, 4 * KiB);
}

struct device_operations soc_amd_i3c_mmio_ops = {
	.read_resources = i3c_read_resources,
	.set_resources = noop_set_resources,
};
