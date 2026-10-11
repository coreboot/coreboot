/* SPDX-License-Identifier: GPL-2.0-or-later */

#include "hob.h"
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/addressmap.h>
#include <stdint.h>

static void add_ram(struct device *dev, unsigned long *index, uint64_t base, uint64_t size)
{
	const uint64_t end = base + size;

	/* The DRAM below AC01_NS_DRAM_START belongs to TF-A */
	if (end <= AC01_NS_DRAM_START)
		return;
	if (base < AC01_NS_DRAM_START)
		base = AC01_NS_DRAM_START;

	ram_range(dev, (*index)++, base, end - base);
}

static void ac01_cpu_cluster_read_resources(struct device *dev)
{
	const struct ac01_platform_info *info = ac01_platform_info();
	unsigned long index = 0;

	if (!info)
		die("No DRAM layout from TF-A\n");

	const struct ac01_dram_info *dram = &info->dram_info;
	bool cbmem_in_dram = false;

	for (unsigned int i = 0; i < dram->num_region; i++) {
		add_ram(dev, &index, dram->base[i], dram->size[i]);

		if (dram->base[i] < cbmem_top() &&
		    dram->base[i] + dram->size[i] >= cbmem_top())
			cbmem_in_dram = true;
	}

	if (!cbmem_in_dram)
		printk(BIOS_ERR, "CBMEM (top 0x%lx) is not in a DRAM region reported by TF-A\n",
		       cbmem_top());
}

static struct device_operations ac01_cpu_cluster_ops = {
	.read_resources = ac01_cpu_cluster_read_resources,
	.set_resources = noop_set_resources,
};

static void ac01_enable_dev(struct device *dev)
{
	switch (dev->path.type) {
	case DEVICE_PATH_CPU_CLUSTER:
		dev->ops = &ac01_cpu_cluster_ops;
		break;
	default:
		break;
	}
}

struct chip_operations soc_ampere_altra_ops = {
	.enable_dev = ac01_enable_dev,
};
