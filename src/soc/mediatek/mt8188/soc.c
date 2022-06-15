/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/emi.h>
#include <soc/mmu_operations.h>
#include <symbols.h>

static void soc_read_resources(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)_dram, sdram_size());
}

static void soc_init(struct device *dev)
{
	mtk_mmu_disable_l2c_sram();
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

struct chip_operations soc_mediatek_mt8188_ops = {
	CHIP_NAME("SOC Mediatek MT8188")
	.enable_dev = enable_soc_dev,
};
