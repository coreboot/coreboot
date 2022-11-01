/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <bootmem.h>
#include <console/console.h>
#include <device/device.h>
#include <soc/cpu_input_gating.h>
#include <soc/devapc.h>
#include <soc/dfd.h>
#include <soc/dpm.h>
#include <soc/emi.h>
#include <soc/mcupm.h>
#include <soc/mmu_operations.h>
#include <soc/spm.h>
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

	disable_cpu_input_gating();

	dapc_init();
	mcupm_init();
	sspm_init();

	if (dpm_init())
		printk(BIOS_ERR, "dpm init failed, DVFS may not work\n");

	/*
	 * For MT8188, SPM will handshake with DPM to do initialization, so
	 * this must run after dpm_init().
	 */
	if (spm_init())
		printk(BIOS_ERR, "spm init failed, system suspend may not work\n");

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

struct chip_operations soc_mediatek_mt8188_ops = {
	CHIP_NAME("SOC Mediatek MT8188")
	.enable_dev = enable_soc_dev,
};
