/* SPDX-License-Identifier: GPL-2.0-only */

#include <symbols.h>
#include <device/device.h>
#include <soc/mmu.h>
#include <soc/symbols.h>

static void soc_read_resources(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)_dram, 1 * GiB);
	reserved_ram_range(dev, 1, (uintptr_t)_dram_reserved, REGION_SIZE(dram_reserved));
}

static void soc_init(struct device *dev)
{

}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.init = soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	dev->ops = &soc_ops;
}

struct chip_operations soc_qualcomm_qcs405_ops = {
	CHIP_NAME("SOC Qualcomm QCS405")
	.enable_dev = enable_soc_dev,
};
