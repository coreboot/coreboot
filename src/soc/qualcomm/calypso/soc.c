/* SPDX-License-Identifier: GPL-2.0-only */

#include <bl31.h>
#include <bootstate.h>
#include <cbfs.h>
#include <device/device.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/symbols_common.h>
#include <soc/pcie.h>
#include <soc/clock.h>
#include <soc/cpucp.h>
#include <soc/qspi_common.h>
#include <soc/variant.h>
#include <program_loading.h>

#define ACDB_CARVEOUT_OFFSET 0x40000000

#define SPI_BUS_CLOCK_FREQ (50 * MHz)

/*
 * FIXME: Reduce SPI Frequency to 50-MHz to improve
 * the platform stability during payload stage.
 */
void soc_prepare_bl31_handoff(void)
{
	printk(BIOS_WARNING, "%s: Reduce SPI frequency to 50MHz to better stability\n",
		 __func__);
	qspi_set_bus_clock(SPI_BUS_CLOCK_FREQ);
}

static void preload_bl31(void)
{
	if (!CONFIG(ARM64_USE_ARM_TRUSTED_FIRMWARE) || !CONFIG(CBFS_PRELOAD))
		return;

	cbfs_preload(CONFIG_CBFS_PREFIX"/bl31");
}

static void preload_bl32(void)
{
	if (!CONFIG(ARM64_USE_SECURE_OS) || !CONFIG(CBFS_PRELOAD))
		return;

	cbfs_preload(CONFIG_CBFS_PREFIX"/secure_os");
}

/*
 * Weak implementation of mainboard-specific display initialization.
 * This can be overridden by mainboard-specific code.
 */
__weak void mainboard_soc_init(void)
{
	/* Default implementation: do nothing */
}

static struct device_operations pci_domain_ops = {
	.read_resources = &qcom_pci_domain_read_resources,
	.set_resources = &pci_domain_set_resources,
	.scan_bus = &pci_host_bridge_scan_bus,
	.enable = &qcom_setup_pcie_host,
};

static uint64_t calc_acdb_carveout_size(void)
{
	return ((((region_sz(ddr_region) / GiB) * 11) / 2 + 1 + 3) * MiB);
}

static uint64_t calc_mte_size(void)
{
	return (((region_sz(ddr_region) / GiB) / 32));
}

static void soc_read_resources(struct device *dev)
{
	int index = 0;
	int count;
	struct region *config = qc_get_soc_dram_space_config(region_sz(ddr_region), &count);

	for (int i = 0; i < count; i++)
		ram_range(dev, index++, (uintptr_t)config[i].offset, config[i].size);

	mmio_range(dev, index++, (uintptr_t)_dram_aop_cmd_db, REGION_SIZE(dram_aop_cmd_db));

	reserved_ram_range(dev, index++, (uintptr_t)_dram_ncc, REGION_SIZE(dram_ncc));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_cpucp, REGION_SIZE(dram_cpucp));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_xbl_log, REGION_SIZE(dram_xbl_log));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_ramdump, REGION_SIZE(dram_ramdump));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_tz, REGION_SIZE(dram_tz));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_tz_ac, REGION_SIZE(dram_tz_ac));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_hyp_ac, REGION_SIZE(dram_hyp_ac));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_aop, REGION_SIZE(dram_aop));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_aop_config, REGION_SIZE(dram_aop_config));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_tme_crashdump, REGION_SIZE(dram_tme_crashdump));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_tme_log, REGION_SIZE(dram_tme_log));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_dc_log, REGION_SIZE(dram_dc_log));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_pdp, REGION_SIZE(dram_pdp));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_pdp_cdb, REGION_SIZE(dram_pdp_cdb));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_pdp_ns, REGION_SIZE(dram_pdp_ns));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_tz_static, REGION_SIZE(dram_tz_static));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_adsp_rpc_heap, REGION_SIZE(dram_adsp_rpc_heap));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_pil, REGION_SIZE(dram_pil));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_ta, REGION_SIZE(dram_ta));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_llcc_lpi, REGION_SIZE(dram_llcc_lpi));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_smem, REGION_SIZE(dram_smem));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_gpu_prr, REGION_SIZE(dram_gpu_prr));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_tpm_ctrl, REGION_SIZE(dram_tpm_ctrl));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_usb_ucsi, REGION_SIZE(dram_usb_ucsi));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_pld_pep, REGION_SIZE(dram_pld_pep));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_pld_gmu, REGION_SIZE(dram_pld_gmu));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_pld_pdp, REGION_SIZE(dram_pld_pdp));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_qcskext, REGION_SIZE(dram_qcskext));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_qup_fw, REGION_SIZE(dram_qup_fw));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_softsku, REGION_SIZE(dram_softsku));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_acpi_ta, REGION_SIZE(dram_acpi_ta));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_xbl_scratch_buf2, REGION_SIZE(dram_xbl_scratch_buf2));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_xbl_scratch_buf1, REGION_SIZE(dram_xbl_scratch_buf1));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_spu_secure, REGION_SIZE(dram_spu_secure));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_smmu_pt, REGION_SIZE(dram_smmu_pt));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_bert, REGION_SIZE(dram_bert));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_oob_glink_always, REGION_SIZE(dram_oob_glink_always));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_oob_glink_assist, REGION_SIZE(dram_oob_glink_assist));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_oob_mdm_assist, REGION_SIZE(dram_oob_mdm_assist));
	reserved_ram_range(dev, index++, (uintptr_t)_dram_oob_wlan_assist, REGION_SIZE(dram_oob_wlan_assist));
	/* ACDB carveout region located at 0x8C0000000 - (n*5.5 + 1 + 3) where n is size of DDR */
	reserved_ram_range(dev, index++,
		(uintptr_t)_dram_space_1 + ACDB_CARVEOUT_OFFSET - calc_acdb_carveout_size(),
		calc_acdb_carveout_size());
	/* MTE is located at 0x8_C000_0000 ++ MTE size */
	reserved_ram_range(dev, index++,
		(uintptr_t)_dram_space_1 + ACDB_CARVEOUT_OFFSET + calc_mte_size(),
		calc_mte_size());
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
	cpucp_pdp_fw_load_reset();
	qtee_fw_config_load();
	preload_bl31();
	preload_bl32();
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

struct chip_operations soc_qualcomm_calypso_ops = {
	.name = "Calypso",
	.enable_dev = enable_soc_dev,
};

static void soc_late_init(void *unused)
{
	/* placeholder code in sync w/ x1p42100 SoC */
	mainboard_soc_init();
}

BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY, soc_late_init, NULL);
