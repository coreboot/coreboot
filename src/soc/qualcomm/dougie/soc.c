/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>

static struct device_operations soc_ops = {
	.read_resources = noop_read_resources,
	.set_resources = noop_set_resources,
};

static void enable_soc_dev(struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &soc_ops;
}

struct chip_operations soc_qualcomm_dougie_ops = {
	.name = "SOC Qualcomm Dougie",
	.enable_dev = enable_soc_dev,
};
