/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>
#include <soc/aop_common.h>
#include <soc/cpucp.h>
#include <soc/pcie.h>

static struct device_operations pci_domain_ops = {
	.read_resources = &qcom_pci_domain_read_resources,
	.set_resources = &pci_domain_set_resources,
	.scan_bus = &pci_domain_scan_bus,
	.enable = &qcom_setup_pcie_host,
};

static void soc_read_resources(struct device *dev)
{
	void *start = NULL;
	void *end = NULL;

	ram_range(dev, 0, (uintptr_t)ddr_region->offset, ddr_region->size);
	reserved_ram_range(dev, 1, (uintptr_t)_dram_soc, REGION_SIZE(dram_soc));
	reserved_ram_range(dev, 2, (uintptr_t)_dram_wlan, REGION_SIZE(dram_wlan));
	reserved_ram_range(dev, 3, (uintptr_t)_dram_wpss, REGION_SIZE(dram_wpss));
	reserved_ram_range(dev, 4, (uintptr_t)_dram_aop, REGION_SIZE(dram_aop));
	reserved_ram_range(dev, 5, (uintptr_t)_dram_cpucp, REGION_SIZE(dram_cpucp));
	if (soc_modem_carve_out(&start, &end))
		reserved_ram_range(dev, 6, (uintptr_t)start, end - start);
}

static void soc_init(struct device *dev)
{
	aop_fw_load_reset();
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
		if (CONFIG(PCI) && CONFIG(NO_ECAM_MMCONF_SUPPORT))
			dev->ops = &pci_domain_ops;
		else
			printk(BIOS_INFO,  "Skip setting PCIe ops\n");
	} else if (dev->path.type == DEVICE_PATH_CPU_CLUSTER) {
		dev->ops = &soc_ops;
	}
}

struct chip_operations soc_qualcomm_sc7280_ops = {
	CHIP_NAME("SOC Qualcomm SC7280")
	.enable_dev = enable_soc_dev,
};
