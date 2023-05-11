/* SPDX-License-Identifier: GPL-2.0-only */

#include <cf9_reset.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <types.h>

#include "eeprom.h"

static const struct cnl_mb_cfg baseboard_mem_cfg = {
	/* Access memory info through SMBUS. */
	.spd[0] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xa0}
	},
	.spd[1] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xa2}
	},
	.spd[2] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xa4}
	},
	.spd[3] = {
		.read_type = READ_SMBUS,
		.spd_spec = {.spd_smbus_address = 0xa6}
	},

	/* Rcomp resistors on CFL-S are located on the CPU itself */
	.rcomp_resistor = {121, 75, 100},

	/* Rcomp target values for CFL-S, DDR4 and 2 DIMMs per channel */
	.rcomp_targets = {60, 26, 20, 20, 26},

	/* Baseboard is an interleaved design */
	.dq_pins_interleaved = 1,

	/* Baseboard is using config 2 for vref_ca */
	.vref_ca_config = 2,

	/* Disable Early Command Training */
	.ect = 0,
};

static bool vmx_needs_full_reset(const bool enable_vmx)
{
	const uint32_t feature_flag = cpu_get_feature_flags_ecx();

	if (!(feature_flag & (CPUID_VMX | CPUID_SMX))) {
		/* CPU does not support VMX, no reset needed */
		return false;
	}

	const msr_t msr = rdmsr(IA32_FEATURE_CONTROL);

	const bool msr_locked  = msr.lo & (1 << 0);
	const bool vmx_enabled = msr.lo & (1 << 2);

	/* Reset if the MSR is locked and VMX state does not match requested state */
	return msr_locked && (vmx_enabled != enable_vmx);
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct eeprom_board_settings *board_cfg = get_board_settings();

	/*
	 * IA32_FEATURE_CONTROL is only unlocked by a full reset.
	 * Check early to avoid wasting time if we need to reset.
	 */
	const bool enable_vmx = !board_cfg->vtx_disabled;
	if (vmx_needs_full_reset(enable_vmx)) {
		printk(BIOS_INFO, "IA32_FEATURE_CONTROL locked; full reset needed to %s VMX\n",
			enable_vmx ? "enable" : "disable");
		full_reset();
	}

	memupd->FspmConfig.UserBd = BOARD_TYPE_SERVER;
	memupd->FspmTestConfig.SmbusSpdWriteDisable = 0;
	cannonlake_memcfg_init(&memupd->FspmConfig, &baseboard_mem_cfg);

	/* Tell FSP-M about the desired primary video adapter so that GGC is set up properly */
	if (board_cfg && board_cfg->primary_video == PRIMARY_VIDEO_INTEL)
		memupd->FspmConfig.PrimaryDisplay = 0; /* iGPU is primary */

	/* Overwrite memupd */
	if (!check_signature(offsetof(struct eeprom_layout, mupd), FSPM_UPD_SIGNATURE))
		return;

	READ_EEPROM_FSP_M(memupd, FspmConfig.RMT);
	READ_EEPROM_FSP_M(memupd, FspmConfig.HyperThreading);
	READ_EEPROM_FSP_M(memupd, FspmConfig.BootFrequency);
	READ_EEPROM_FSP_M(memupd, FspmTestConfig.VtdDisable);
}
