/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmem.h>
#include <device/device.h>
#include <soc/devapc.h>
#include <soc/dfd.h>
#include <soc/emi.h>
#include <soc/mmu_operations.h>
#include <soc/sspm.h>
#include <symbols.h>

void bootmem_platform_add_ranges(void)
{
	if (CONFIG(MTK_DFD))
		bootmem_add_range(DFD_DUMP_ADDRESS, DFD_DUMP_SIZE, BM_MEM_RESERVED);
}

static void soc_read_resources(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)_dram, sdram_size());
}

static void soc_init(struct device *dev)
{
	mtk_mmu_disable_l2c_sram();
	sspm_init();
	dapc_init();

	if (CONFIG(MTK_DFD))
		dfd_init();
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

struct chip_operations soc_mediatek_mt8186_ops = {
	CHIP_NAME("SOC Mediatek MT8186")
	.enable_dev = enable_soc_dev,
};
