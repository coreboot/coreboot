/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <cbmem.h>
#include <cpu/x86/cache.h>
#include <cpu/amd/mtrr.h>
#include <console/uart.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <program_loading.h>
#include <elog.h>
#include <soc/acpi.h>
#include <soc/memmap.h>
#include <soc/mrc_cache.h>
#include <soc/pci_devs.h>
#include <types.h>
#include "chip.h"
#include <fsp/api.h>

static struct chipset_state chipset_state;

static void fill_chipset_state(void)
{
	acpi_fill_pm_gpe_state(&chipset_state.gpe_state);
	gpio_fill_wake_state(&chipset_state.gpio_state);
}

static void add_chipset_state_cbmem(int unused)
{
	struct chipset_state *state;

	state = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*state));

	if (state)
		memcpy(state, &chipset_state, sizeof(*state));
}

ROMSTAGE_CBMEM_INIT_HOOK(add_chipset_state_cbmem);

static const struct device_path hda_path[] = {
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = PCIE_GPP_A_DEVFN
	},
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = HD_AUDIO_DEVFN
	},
};

static bool devtree_hda_dev_enabled(void)
{
	const struct device *hda_dev;

	hda_dev = find_dev_nested_path(pci_root_bus(), hda_path, ARRAY_SIZE(hda_path));

	if (!hda_dev)
		return false;

	return hda_dev->enabled;
}


static const struct device_path sata_path[] = {
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = PCIE_GPP_B_DEVFN
	},
	{
		.type = DEVICE_PATH_PCI,
		.pci.devfn = SATA_DEVFN
	},
};

static bool devtree_sata_dev_enabled(void)
{
	const struct device *ahci_dev;

	ahci_dev = find_dev_nested_path(pci_root_bus(), sata_path, ARRAY_SIZE(sata_path));

	if (!ahci_dev)
		return false;

	return ahci_dev->enabled;
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *mcfg = &mupd->FspmConfig;
	const config_t *config = config_of_soc();

	mupd->FspmArchUpd.NvsBufferPtr = soc_fill_mrc_cache();

	mcfg->pci_express_base_addr = CONFIG_MMCONF_BASE_ADDRESS;
	mcfg->tseg_size = CONFIG_SMM_TSEG_SIZE;
	mcfg->bert_size = CONFIG_ACPI_BERT_SIZE;
	mcfg->serial_port_base = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	mcfg->serial_port_use_mmio = CONFIG(DRIVERS_UART_8250MEM);
	mcfg->serial_port_stride = CONFIG(DRIVERS_UART_8250MEM_32) ? 4 : 1;
	mcfg->serial_port_baudrate = get_uart_baudrate();
	mcfg->serial_port_refclk = uart_platform_refclk();

	mcfg->system_config = config->system_config;

	if ((config->slow_ppt_limit_mW) &&
		(config->fast_ppt_limit_mW) &&
		(config->slow_ppt_time_constant_s) &&
		(config->stapm_time_constant_s)) {
		mcfg->slow_ppt_limit_mW = config->slow_ppt_limit_mW;
		mcfg->fast_ppt_limit_mW = config->fast_ppt_limit_mW;
		mcfg->slow_ppt_time_constant_s = config->slow_ppt_time_constant_s;
		mcfg->stapm_time_constant_s = config->stapm_time_constant_s;
	}

	mcfg->ccx_down_core_mode = config->downcore_mode;
	mcfg->ccx_disable_smt = config->smt_disable;

	mcfg->sustained_power_limit_mW = config->sustained_power_limit_mW;
	mcfg->prochot_l_deassertion_ramp_time_ms = config->prochot_l_deassertion_ramp_time_ms;
	mcfg->thermctl_limit_degreeC = config->thermctl_limit_degreeC;
	mcfg->psi0_current_limit_mA = config->psi0_current_limit_mA;
	mcfg->psi0_soc_current_limit_mA = config->psi0_soc_current_limit_mA;
	mcfg->vddcr_soc_voltage_margin_mV = config->vddcr_soc_voltage_margin_mV;
	mcfg->vddcr_vdd_voltage_margin_mV = config->vddcr_vdd_voltage_margin_mV;
	mcfg->vrm_maximum_current_limit_mA = config->vrm_maximum_current_limit_mA;
	mcfg->vrm_soc_maximum_current_limit_mA = config->vrm_soc_maximum_current_limit_mA;
	mcfg->vrm_current_limit_mA = config->vrm_current_limit_mA;
	mcfg->vrm_soc_current_limit_mA = config->vrm_soc_current_limit_mA;
	mcfg->sb_tsi_alert_comparator_mode_en = config->sb_tsi_alert_comparator_mode_en;
	mcfg->core_dldo_bypass = config->core_dldo_bypass;
	mcfg->min_soc_vid_offset = config->min_soc_vid_offset;
	mcfg->aclk_dpm0_freq_400MHz = config->aclk_dpm0_freq_400MHz;
	mcfg->telemetry_vddcr_vdd_slope_mA = config->telemetry_vddcr_vdd_slope_mA;
	mcfg->telemetry_vddcr_vdd_offset = config->telemetry_vddcr_vdd_offset;
	mcfg->telemetry_vddcr_soc_slope_mA = config->telemetry_vddcr_soc_slope_mA;
	mcfg->telemetry_vddcr_soc_offset = config->telemetry_vddcr_soc_offset;
	mcfg->hd_audio_enable = devtree_hda_dev_enabled();
	mcfg->sata_enable = devtree_sata_dev_enabled();
}

asmlinkage void car_stage_entry(void)
{
	int s3_resume;

	post_code(0x40);
	console_init();

	post_code(0x41);
	s3_resume = acpi_s3_resume_allowed() && acpi_is_wakeup_s3();

	post_code(0x42);
	u32 val = cpuid_eax(1);
	printk(BIOS_DEBUG, "Family_Model: %08x\n", val);

	/* Snapshot chipset state prior to any FSP call. */
	fill_chipset_state();

	post_code(0x43);
	fsp_memory_init(s3_resume);
	soc_update_mrc_cache();

	memmap_stash_early_dram_usage();

	post_code(0x44);
	run_ramstage();

	post_code(0x50); /* Should never see this post code. */
}
