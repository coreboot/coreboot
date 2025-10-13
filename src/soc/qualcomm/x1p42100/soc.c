/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>
#include <soc/pcie.h>
#include <soc/cpucp.h>
#include <program_loading.h>

static struct device_operations pci_domain_ops = {
	.read_resources = &qcom_pci_domain_read_resources,
	.set_resources = &pci_domain_set_resources,
	.scan_bus = &pci_host_bridge_scan_bus,
	.enable = &qcom_setup_pcie_host,
};

static uint64_t calc_acdb_carveout_size(void)
{
	return ((((region_sz(ddr_region) / GiB) * 11) / 2 + 36) * MiB);
}

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

	/* ACDB carveout region located at 0xFF800000 - (n*5.5 +1+32+3) where n is size of DDR */
	reserved_ram_range(dev, index++, (uintptr_t)(_dram_llcc_lpi - calc_acdb_carveout_size()),
			calc_acdb_carveout_size());
	reserved_ram_range(dev, index++, (uintptr_t)_dram_llcc_lpi, REGION_SIZE(dram_llcc_lpi));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_smem, REGION_SIZE(dram_smem));
}

static void qtee_fw_config_load(void)
{
	if (!CONFIG(ARM64_USE_SECURE_OS))
		return;

	struct prog devcfg_tz = PROG_INIT(PROG_PAYLOAD,
					CONFIG_CBFS_PREFIX"/tzoem_cfg");
	if (!selfload(&devcfg_tz))
		die("devcfg_tz load failed");

	struct prog tzqti_cfg = PROG_INIT(PROG_PAYLOAD,
					CONFIG_CBFS_PREFIX"/tzqti_cfg");
	if (!selfload(&tzqti_cfg))
		die("tzqti_cfg load failed");

	struct prog tzac_cfg = PROG_INIT(PROG_PAYLOAD,
					CONFIG_CBFS_PREFIX"/tzac_cfg");
	if (!selfload(&tzac_cfg))
		die("tzac_cfg load failed");

	struct prog hypac_cfg = PROG_INIT(PROG_PAYLOAD,
					CONFIG_CBFS_PREFIX"/hypac_cfg");
	if (!selfload(&hypac_cfg))
		die("hypac_cfg load failed");
}

static void soc_init(struct device *dev)
{
	cpucp_fw_load_reset();
	qtee_fw_config_load();
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
