/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>
#include <soc/pcie.h>
#include <soc/cpucp.h>

static struct device_operations pci_domain_ops = {
	.read_resources = &qcom_pci_domain_read_resources,
	.set_resources = &pci_domain_set_resources,
	.scan_bus = &pci_host_bridge_scan_bus,
	.enable = &qcom_setup_pcie_host,
};

static void soc_read_resources(struct device *dev)
{
	int index = 0;
	int count;
	struct region *config = qc_get_soc_dram_space_config(region_sz(ddr_region),
				 &count);

	for (int i = 0; i < count; i++)
		ram_range(dev, index++, (uintptr_t)config[i].offset, config[i].size);

	reserved_ram_range(dev, index++, (uintptr_t)_dram_ncc, REGION_SIZE(dram_ncc));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_cpucp, REGION_SIZE(dram_cpucp));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_xbl_log, REGION_SIZE(dram_xbl_log));

	reserved_ram_range(dev, index++, (uintptr_t)_dram_ramdump, REGION_SIZE(dram_ramdump));

	reserved_ram_range(dev, index++, (uintptr_t)_dram_tz, REGION_SIZE(dram_tz));

	reserved_ram_range(dev, index++, (uintptr_t)_dram_aop, REGION_SIZE(dram_aop));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_tme_crashdump, REGION_SIZE(dram_tme_crashdump));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_tme_log, REGION_SIZE(dram_tme_log));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_dc_log, REGION_SIZE(dram_dc_log));

	reserved_ram_range(dev, index++, (uintptr_t)_dram_pdp, REGION_SIZE(dram_pdp));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_tz_static, REGION_SIZE(dram_tz_static));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_adsp_rpc_heap, REGION_SIZE(dram_adsp_rpc_heap));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_wlan, REGION_SIZE(dram_wlan));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_pil, REGION_SIZE(dram_pil));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_ta, REGION_SIZE(dram_ta));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_llcc_lpi, REGION_SIZE(dram_llcc_lpi));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_smem, REGION_SIZE(dram_smem));
}

static void soc_init(struct device *dev)
{
	cpucp_fw_load_reset();
}

static struct device_operations soc_ops = {
	.read_resources = soc_read_resources,
	.set_resources = noop_set_resources,
	.init = soc_init,
};

static void enable_soc_dev(struct device *dev)
{
	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_DOMAIN) {
		if (mainboard_needs_pcie_init())
			dev->ops = &pci_domain_ops;
		else
			printk(BIOS_DEBUG, "Skip setting PCIe ops\n");
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER)
		dev->ops = &soc_ops;
}

struct chip_operations soc_qualcomm_x1p42100_ops = {
	.name = "SOC Qualcomm X1P-42-100",
	.enable_dev = enable_soc_dev,
};
