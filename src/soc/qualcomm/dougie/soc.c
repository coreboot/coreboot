/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>

static void soc_read_resources(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)region_offset(ddr_region), region_sz(ddr_region));
	reserved_ram_range(dev, 1, (uintptr_t)_dram_soc, REGION_SIZE(dram_soc));
	reserved_ram_range(dev, 2, (uintptr_t)_dram_wlan, REGION_SIZE(dram_wlan));
	reserved_ram_range(dev, 3, (uintptr_t)_dram_wpss, REGION_SIZE(dram_wpss));
	reserved_ram_range(dev, 4, (uintptr_t)_dram_aop, REGION_SIZE(dram_aop));
	reserved_ram_range(dev, 5, (uintptr_t)_dram_cpucp, REGION_SIZE(dram_cpucp));
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
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
