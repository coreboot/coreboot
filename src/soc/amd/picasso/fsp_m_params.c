/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/apob_cache.h>
#include <commonlib/helpers.h>
#include <console/uart.h>
#include <device/device.h>
#include <fsp/api.h>
#include <soc/platform_descriptors.h>
#include <soc/pci_devs.h>
#include <soc/fsp.h>
#include <types.h>
#include "chip.h"

void __weak mainboard_updm_update(FSP_M_CONFIG *mupd) {}

void __weak mb_pre_fspm(void) {}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *mcfg = &mupd->FspmConfig;
	const struct soc_amd_picasso_config *config = config_of_soc();

	mupd->FspmArchUpd.NvsBufferPtr = (uintptr_t)soc_fill_apob_cache();

	mcfg->pci_express_base_addr = CONFIG_ECAM_MMCONF_BASE_ADDRESS;
	mcfg->tseg_size = CONFIG_SMM_TSEG_SIZE;
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
	mcfg->hd_audio_enable = is_dev_enabled(DEV_PTR(hda));
	mcfg->sata_enable = is_dev_enabled(DEV_PTR(sata));
	mcfg->hdmi2_disable = config->hdmi2_disable;

	/* PCIe power vs. speed */
	mcfg->pspp_policy = config->pspp_policy;

	mainboard_updm_update(mcfg);
	mb_pre_fspm();
}
