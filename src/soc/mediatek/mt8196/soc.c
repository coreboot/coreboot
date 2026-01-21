/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmem.h>
#include <bootstate.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/booker.h>
#include <soc/dcc.h>
#include <soc/dpm_v2.h>
#include <soc/dramc_info.h>
#include <soc/emi.h>
#include <soc/gpueb.h>
#include <soc/mcupm.h>
#include <soc/mmu_operations.h>
#include <soc/mt6685.h>
#include <soc/mtk_fsp.h>
#include <soc/pcie.h>
#include <soc/pi_image.h>
#include <soc/spm.h>
#include <soc/sspm.h>
#include <soc/storage.h>
#include <soc/symbols.h>
#include <symbols.h>

static uint64_t mte_start;
static size_t mte_size;

void bootmem_platform_add_ranges(void)
{
	if (CONFIG(ARM64_BL31_OPTEE_WITH_SMC))
		bootmem_add_range((uint64_t)_resv_mem_optee,
				  REGION_SIZE(resv_mem_optee), BM_MEM_RESERVED);

	reserve_buffer_for_dramc();

	bootmem_add_range((uint64_t)_resv_mem_gpu, REGION_SIZE(resv_mem_gpu), BM_MEM_RESERVED);
	bootmem_add_range((uint64_t)_resv_mem_gpueb,
			  REGION_SIZE(resv_mem_gpueb), BM_MEM_RESERVED);

	bootmem_add_range_from(mte_start, mte_size, BM_MEM_TAG, BM_MEM_RAM);
}

static void soc_read_resources(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)_dram, sdram_size());
}

#define MTE_SIZE_ALIGNMENT	(64 * KiB)

static void mte_setup(void)
{
	size_t dram_size = sdram_size();

	mte_size = ALIGN_UP(dram_size / 33, MTE_SIZE_ALIGNMENT);
	mte_start = ALIGN_DOWN((uint64_t)_dram + dram_size - mte_size - HW_TX_TRACING_BUF_SIZE,
			       MTE_SIZE_ALIGNMENT);
	booker_mte_init(mte_start);
}

static void fsp_init(void *arg)
{
	uint32_t storage_type = mainboard_get_storage_type();

	dcc_init();
	mtk_fsp_init(RAMSTAGE_SOC_INIT);
	mtk_fsp_add_param(FSP_PARAM_TYPE_STORAGE, sizeof(storage_type), &storage_type);
	pi_image_add_mtk_fsp_params();
	mtk_fsp_load_and_run();
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_ENTRY, fsp_init, NULL);

static void soc_init(struct device *dev)
{
	mtk_mmu_disable_l2c_sram();

	if (dpm_init())
		printk(BIOS_ERR, "dpm init failed, DVFS may not work\n");
	if (spm_init())
		printk(BIOS_ERR, "spm init failed, Suspend may not work\n");

	sspm_init();
	gpueb_init();
	mcupm_init();
	mt6685_init_pmif_arb();
	/*
	 * According to CI-700 documentation:
	 * Registers are only accessible by Secure accesses. Writes to them must occur prior to
	 * the first non-configuration access targeting the device.
	 */
	mte_setup();
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.set_resources = noop_set_resources,
	.init = soc_init,
};

static struct device_operations pci_domain_ops = {
	.read_resources = &mtk_pcie_domain_read_resources,
	.set_resources = &mtk_pcie_domain_set_resources,
	.scan_bus = &pci_host_bridge_scan_bus,
	.enable = &mtk_pcie_domain_enable,
};

static struct device_operations noop_domain_ops = {
	.read_resources = &noop_read_resources,
	.set_resources = &noop_set_resources,
};

static void enable_soc_dev(struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &soc_ops;
	} else if (dev->path.type == DEVICE_PATH_DOMAIN) {
		if (mainboard_needs_pcie_init()) {
			dev->ops = &pci_domain_ops;
		} else {
			printk(BIOS_DEBUG, "Skip setting PCIe ops\n");
			dev->ops = &noop_domain_ops;
		}
	}
}

struct chip_operations soc_mediatek_mt8196_ops = {
	.name = "SOC Mediatek MT8196",
	.enable_dev = enable_soc_dev,
};
