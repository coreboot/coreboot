/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpi.h>
#include <amdblocks/biosram.h>
#include <device/pci_ops.h>
#include <arch/cpu.h>
#include <arch/romstage.h>
#include <acpi/acpi.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <cpu/amd/mtrr.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <elog.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <soc/southbridge.h>
#include <amdblocks/psp.h>
#include <stdint.h>

#include "chip.h"

void __weak mainboard_romstage_entry(void)
{
	/* By default, don't do anything */
}

static void agesa_call(void)
{
	post_code(0x37);
	do_agesawrapper(AMD_INIT_RESET, "amdinitreset");

	post_code(0x38);
	/* APs will not exit amdinitearly */
	do_agesawrapper(AMD_INIT_EARLY, "amdinitearly");
}

static void bsp_agesa_call(void)
{
	set_ap_entry_ptr(agesa_call); /* indicate the path to the AP */
	agesa_call();
}

asmlinkage void car_stage_entry(void)
{
	msr_t base, mask;
	msr_t mtrr_cap = rdmsr(MTRR_CAP_MSR);
	int vmtrrs = mtrr_cap.lo & MTRR_CAP_VCNT;
	int s3_resume = acpi_is_wakeup_s3();
	int i;

	console_init();

	soc_enable_psp_early();
	if (CONFIG(SOC_AMD_PSP_SELECTABLE_SMU_FW))
		psp_load_named_blob(BLOB_SMU_FW, "smu_fw");

	mainboard_romstage_entry();
	elog_boot_notify(s3_resume);

	bsp_agesa_call();

	if (!s3_resume) {
		post_code(0x40);
		do_agesawrapper(AMD_INIT_POST, "amdinitpost");

		post_code(0x41);
		/*
		 * TODO: This is a hack to work around current AGESA behavior.
		 *       AGESA needs to change to reflect that coreboot owns
		 *       the MTRRs.
		 *
		 * After setting up DRAM, AGESA also completes the configuration
		 * of the MTRRs, setting regions to WB.  Anything written to
		 * memory between now and when CAR is dismantled will be
		 * in cache and lost.  For now, set the regions UC to ensure
		 * the writes get to DRAM.
		 */
		for (i = 0 ; i < vmtrrs ; i++) {
			base = rdmsr(MTRR_PHYS_BASE(i));
			mask = rdmsr(MTRR_PHYS_MASK(i));
			if (!(mask.lo & MTRR_PHYS_MASK_VALID))
				continue;

			if ((base.lo & 0x7) == MTRR_TYPE_WRBACK) {
				base.lo &= ~0x7;
				base.lo |= MTRR_TYPE_UNCACHEABLE;
				wrmsr(MTRR_PHYS_BASE(i), base);
			}
		}
		/* Disable WB from to region 4GB-TOM2. */
		msr_t sys_cfg = rdmsr(SYSCFG_MSR);
		sys_cfg.lo &= ~SYSCFG_MSR_TOM2WB;
		wrmsr(SYSCFG_MSR, sys_cfg);
	} else {
		printk(BIOS_INFO, "S3 detected\n");
		post_code(0x60);
		do_agesawrapper(AMD_INIT_RESUME, "amdinitresume");

		post_code(0x61);
	}

	post_code(0x42);
	psp_notify_dram();

	post_code(0x43);
	if (cbmem_recovery(s3_resume))
		printk(BIOS_CRIT, "Failed to recover cbmem\n");
	if (romstage_handoff_init(s3_resume))
		printk(BIOS_ERR, "Failed to set romstage handoff data\n");

	if (CONFIG(SMM_TSEG))
		smm_list_regions();

	post_code(0x44);
	prepare_and_run_postcar();
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram = (uintptr_t)cbmem_top();
	postcar_frame_add_mtrr(pcf, top_of_ram - 16 * MiB, 16 * MiB, MTRR_TYPE_WRBACK);

	/* Cache the TSEG region */
	postcar_enable_tseg_cache(pcf);
}

void SetMemParams(AMD_POST_PARAMS *PostParams)
{
	const struct soc_amd_stoneyridge_config *cfg;
	const struct device *dev = pcidev_path_on_root(GNB_DEVFN);

	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "Cannot find SoC devicetree config\n");
		/* In case of a BIOS error, only attempt to set UMA. */
		PostParams->MemConfig.UmaMode = CONFIG(GFXUMA) ?
					UMA_AUTO : UMA_NONE;
		return;
	}

	cfg = dev->chip_info;

	PostParams->MemConfig.EnableMemClr = cfg->dram_clear_on_reset;

	switch (cfg->uma_mode) {
	case UMAMODE_NONE:
		PostParams->MemConfig.UmaMode = UMA_NONE;
		break;
	case UMAMODE_SPECIFIED_SIZE:
		PostParams->MemConfig.UmaMode = UMA_SPECIFIED;
		/* 64 KiB blocks. */
		PostParams->MemConfig.UmaSize = cfg->uma_size / (64 * KiB);
		break;
	case UMAMODE_AUTO_LEGACY:
		PostParams->MemConfig.UmaMode = UMA_AUTO;
		PostParams->MemConfig.UmaVersion = UMA_LEGACY;
		break;
	case UMAMODE_AUTO_NON_LEGACY:
		PostParams->MemConfig.UmaMode = UMA_AUTO;
		PostParams->MemConfig.UmaVersion = UMA_NON_LEGACY;
		break;
	}
}

void soc_customize_init_early(AMD_EARLY_PARAMS *InitEarly)
{
	const struct soc_amd_stoneyridge_config *cfg;
	const struct device *dev = pcidev_path_on_root(GNB_DEVFN);
	struct _PLATFORM_CONFIGURATION *platform;

	if (!dev || !dev->chip_info) {
		printk(BIOS_WARNING, "Cannot find SoC devicetree"
					" config, STAPM unchanged\n");
		return;
	}
	cfg = dev->chip_info;
	platform = &InitEarly->PlatformConfig;
	if ((cfg->stapm_percent) && (cfg->stapm_time_ms) &&
				    (cfg->stapm_power_mw)) {
		platform->PlatStapmConfig.CfgStapmScalar = cfg->stapm_percent;
		platform->PlatStapmConfig.CfgStapmTimeConstant =
							cfg->stapm_time_ms;
		platform->PkgPwrLimitDC = cfg->stapm_power_mw;
		platform->PkgPwrLimitAC = cfg->stapm_power_mw;
		platform->PlatStapmConfig.CfgStapmBoost = StapmBoostEnabled;
	}
}

static void migrate_power_state(int is_recovery)
{
	struct chipset_power_state *state;
	state = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*state));
	if (state) {
		acpi_fill_pm_gpe_state(&state->gpe_state);
		acpi_pm_gpe_add_events_print_events();
	}
}
CBMEM_CREATION_HOOK(migrate_power_state);
