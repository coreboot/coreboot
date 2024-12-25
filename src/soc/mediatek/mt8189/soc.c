/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <bootmem.h>
#include <device/device.h>
#include <soc/dramc_info.h>
#include <soc/emi.h>
#include <symbols.h>

void bootmem_platform_add_ranges(void)
{
	reserve_buffer_for_dramc();
}

static void soc_read_resources(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)_dram, sdram_size());
}

static void soc_init(struct device *dev)
{
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.set_resources = noop_set_resources,
	.init = soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_mediatek_mt8189_ops = {
	.name = "SOC Mediatek MT8189",
	.enable_dev = enable_soc_dev,
};
