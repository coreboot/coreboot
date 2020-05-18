/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <acpi/acpi.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <console/uart.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <elog.h>
#include <soc/romstage.h>
#include <soc/mtrr.h>
#include <types.h>
#include "chip.h"
#include <fsp/api.h>

void __weak mainboard_romstage_entry_s3(int s3_resume)
{
	/* By default, don't do anything */
}

/* TODO(b/155426691): Make FSP AGESA leave MTRRs alone */
static void clear_agesa_mtrrs(void)
{
	disable_cache();

	picasso_restore_mtrrs();

	enable_cache();
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	FSP_M_CONFIG *mcfg = &mupd->FspmConfig;
	const config_t *config = config_of_soc();

	mcfg->pci_express_base_addr = CONFIG_MMCONF_BASE_ADDRESS;
	mcfg->tseg_size = CONFIG_SMM_TSEG_SIZE;
	mcfg->serial_port_base = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	mcfg->serial_port_use_mmio = CONFIG(DRIVERS_UART_8250MEM);
	mcfg->serial_port_stride = CONFIG(DRIVERS_UART_8250MEM_32) ? 4 : 1;
	mcfg->serial_port_baudrate = get_uart_baudrate();
	mcfg->serial_port_refclk = uart_platform_refclk();

	mcfg->system_config = config->system_config;

	if ((config->slow_ppt_limit) &&
		(config->fast_ppt_limit) &&
		(config->slow_ppt_time_constant) &&
		(config->stapm_time_constant)) {
		mcfg->slow_ppt_limit = config->slow_ppt_limit;
		mcfg->fast_ppt_limit = config->fast_ppt_limit;
		mcfg->slow_ppt_time_constant = config->slow_ppt_time_constant;
		mcfg->stapm_time_constant = config->stapm_time_constant;
	}

	mcfg->sustained_power_limit = config->sustained_power_limit;
	mcfg->prochot_l_deassertion_ramp_time = config->prochot_l_deassertion_ramp_time;
	mcfg->thermctl_limit = config->thermctl_limit;
	mcfg->psi0_current_limit = config->psi0_current_limit;
	mcfg->psi0_soc_current_limit = config->psi0_soc_current_limit;
	mcfg->vddcr_soc_voltage_margin = config->vddcr_soc_voltage_margin;
	mcfg->vddcr_vdd_voltage_margin = config->vddcr_vdd_voltage_margin;
	mcfg->vrm_maximum_current_limit = config->vrm_maximum_current_limit;
	mcfg->vrm_soc_maximum_current_limit = config->vrm_soc_maximum_current_limit;
	mcfg->vrm_current_limit = config->vrm_current_limit;
	mcfg->vrm_soc_current_limit = config->vrm_soc_current_limit;
	mcfg->sb_tsi_alert_comparator_mode_en = config->sb_tsi_alert_comparator_mode_en;
	mcfg->core_dldo_bypass = config->core_dldo_bypass;
	mcfg->min_soc_vid_offset = config->min_soc_vid_offset;
	mcfg->aclk_dpm0_freq_400MHz = config->aclk_dpm0_freq_400MHz;
	mcfg->telemetry_vddcr_vdd_slope = config->telemetry_vddcr_vdd_slope;
	mcfg->telemetry_vddcr_vdd_offset = config->telemetry_vddcr_vdd_offset;
	mcfg->telemetry_vddcr_soc_slope = config->telemetry_vddcr_soc_slope;
	mcfg->telemetry_vddcr_soc_offset = config->telemetry_vddcr_soc_offset;
}

asmlinkage void car_stage_entry(void)
{
	int s3_resume;

	post_code(0x40);
	console_init();

	post_code(0x41);
	s3_resume = acpi_s3_resume_allowed() && acpi_is_wakeup_s3();
	mainboard_romstage_entry_s3(s3_resume);
	elog_boot_notify(s3_resume);

	post_code(0x42);
	u32 val = cpuid_eax(1);
	printk(BIOS_DEBUG, "Family_Model: %08x\n", val);

	post_code(0x43);
	picasso_save_mtrrs();

	post_code(0x44);
	fsp_memory_init(s3_resume);

	post_code(0x45);
	clear_agesa_mtrrs();

	post_code(0x46);
	run_ramstage();

	post_code(0x50); /* Should never see this post code. */
}
