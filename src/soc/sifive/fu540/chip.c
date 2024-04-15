/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <device/device.h>
#include <soc/addressmap.h>

static void fu540_read_resources(struct device *dev)
{
	int index = 0;
	ram_from_to(dev, index++, FU540_DRAM, (uintptr_t)cbmem_top());
}

struct device_operations fu540_cpu_ops = {
	.read_resources = fu540_read_resources,
};

struct chip_operations soc_sifive_fu540_ops = {
	.name = "SIFIVE FU540",
};
