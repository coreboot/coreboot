/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmem.h>
#include <device/device.h>
#include <device/pci.h>
#include <soc/dramc_info.h>
#include <soc/emi.h>
#include <soc/gpueb.h>
#include <soc/mcupm.h>
#include <soc/mmu_operations.h>
#include <soc/mt6685.h>
#include <soc/mtk_fsp.h>
#include <soc/pcie.h>
#include <soc/pi_image.h>
#include <soc/sspm.h>
#include <soc/storage.h>
#include <soc/symbols.h>
#include <symbols.h>

void bootmem_platform_add_ranges(void)
{
	if (CONFIG(ARM64_BL31_OPTEE_WITH_SMC))
		bootmem_add_range((uint64_t)_resv_mem_optee,
				  REGION_SIZE(resv_mem_optee), BM_MEM_RESERVED);

	reserve_buffer_for_dramc();

	bootmem_add_range((uint64_t)_resv_mem_gpu, REGION_SIZE(resv_mem_gpu), BM_MEM_RESERVED);
	bootmem_add_range((uint64_t)_resv_mem_gpueb,
			  REGION_SIZE(resv_mem_gpueb), BM_MEM_RESERVED);
}

static void soc_read_resources(struct device *dev)
{
	ram_range(dev, 0, (uintptr_t)_dram, sdram_size());
}

static void add_pi_image_params(void)
{
	void *pi_image;
	size_t pi_image_size;
	pi_image_size = pi_image_load(&pi_image);

	void *csram = (void *)PI_IMAGE_CSRAM;
	size_t csram_size = PI_IMAGE_CSRAM_SIZE;

	mtk_fsp_add_param(FSP_PARAM_TYPE_PI_IMG, pi_image_size, pi_image);
	mtk_fsp_add_param(FSP_PARAM_TYPE_PI_IMG_CSRAM, csram_size, csram);
}

static void soc_init(struct device *dev)
{
	uint32_t storage_type = mainboard_get_storage_type();

	mtk_fsp_init(RAMSTAGE_SOC_INIT);
	mtk_fsp_add_param(FSP_PARAM_TYPE_STORAGE, sizeof(storage_type), &storage_type);
	add_pi_image_params();
	mtk_fsp_load_and_run();

	mtk_mmu_disable_l2c_sram();
	sspm_init();
	gpueb_init();
	mcupm_init();
	mt6685_init_pmif_arb();
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

static void enable_soc_dev(struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &soc_ops;
	} else if (dev->path.type == DEVICE_PATH_DOMAIN) {
		if (mainboard_needs_pcie_init())
			dev->ops = &pci_domain_ops;
		else
			printk(BIOS_DEBUG, "Skip setting PCIe ops\n");
	}
}

struct chip_operations soc_mediatek_mt8196_ops = {
	.name = "SOC Mediatek MT8196",
	.enable_dev = enable_soc_dev,
};
