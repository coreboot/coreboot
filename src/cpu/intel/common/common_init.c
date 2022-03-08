/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <cpu/intel/msr.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/turbo.h>
#include "common.h"

#define  CPUID_6_ECX_EPB		(1 << 3)
#define  CPUID_6_ENGERY_PERF_PREF	(1 << 10)
#define  CPUID_6_HWP			(1 << 7)

void set_vmx_and_lock(void)
{
	set_feature_ctrl_vmx();
	set_feature_ctrl_lock();
}

void set_feature_ctrl_vmx_arg(bool enable)
{
	msr_t msr;
	uint32_t feature_flag;

	feature_flag = cpu_get_feature_flags_ecx();
	/* Check that the VMX is supported before reading or writing the MSR. */
	if (!((feature_flag & CPUID_VMX) || (feature_flag & CPUID_SMX))) {
		printk(BIOS_DEBUG, "CPU doesn't support VMX; exiting\n");
		return;
	}

	msr = rdmsr(IA32_FEATURE_CONTROL);

	if (msr.lo & (1 << 0)) {
		printk(BIOS_DEBUG, "IA32_FEATURE_CONTROL already locked; ");
		printk(BIOS_DEBUG, "VMX status: %s\n", msr.lo & (1 << 2)  ?
			"enabled" : "disabled");
		/* IA32_FEATURE_CONTROL locked. If we set it again we get an
		 * illegal instruction
		 */
		return;
	}

	/* The IA32_FEATURE_CONTROL MSR may initialize with random values.
	 * It must be cleared regardless of VMX config setting.
	 */
	msr.hi = msr.lo = 0;

	if (enable) {
		msr.lo |= (1 << 2);
		if (feature_flag & CPUID_SMX) {
			msr.lo |= (1 << 1);
			if (CONFIG(INTEL_TXT)) {
				/* Enable GetSec and all GetSec leaves */
				msr.lo |= (0xff << 8);
			}
		}
	}

	wrmsr(IA32_FEATURE_CONTROL, msr);

	printk(BIOS_DEBUG, "VMX status: %s\n",
		enable ? "enabled" : "disabled");
}

void set_feature_ctrl_vmx(void)
{
	set_feature_ctrl_vmx_arg(CONFIG(ENABLE_VMX));
}

void set_feature_ctrl_lock(void)
{
	msr_t msr;
	int lock = CONFIG(SET_IA32_FC_LOCK_BIT);
	uint32_t feature_flag = cpu_get_feature_flags_ecx();

	/* Check if VMX is supported before reading or writing the MSR */
	if (!((feature_flag & CPUID_VMX) || (feature_flag & CPUID_SMX))) {
		printk(BIOS_DEBUG, "Read IA32_FEATURE_CONTROL unsupported\n");
		return;
	}

	msr = rdmsr(IA32_FEATURE_CONTROL);

	if (msr.lo & (1 << 0)) {
		printk(BIOS_DEBUG, "IA32_FEATURE_CONTROL already locked\n");
		/* IA32_FEATURE_CONTROL locked. If we set it again we get an
		 * illegal instruction
		 */
		return;
	}

	if (lock) {
		/* Set lock bit */
		msr.lo |= (1 << 0);
		wrmsr(IA32_FEATURE_CONTROL, msr);
	}

	printk(BIOS_DEBUG, "IA32_FEATURE_CONTROL status: %s\n",
		lock ? "locked" : "unlocked");
}

/*
 * Init cppc_config in a way that's appropriate for Intel
 * processors with Intel Enhanced Speed Step Technology.
 * NOTE: version 2 is expected to be the typical use case.
 * For now this function 'punts' on version 3 and just
 * populates the additional fields with 'unsupported'.
 */
void cpu_init_cppc_config(struct cppc_config *config, u32 version)
{
	config->version = version;

	config->entries[CPPC_HIGHEST_PERF]		= CPPC_REG_MSR(IA32_HWP_CAPABILITIES, 0, 8);
	config->entries[CPPC_NOMINAL_PERF]		= CPPC_REG_MSR(MSR_PLATFORM_INFO, 8, 8);
	config->entries[CPPC_LOWEST_NONL_PERF]		= CPPC_REG_MSR(IA32_HWP_CAPABILITIES, 16, 8);
	config->entries[CPPC_LOWEST_PERF]		= CPPC_REG_MSR(IA32_HWP_CAPABILITIES, 24, 8);
	config->entries[CPPC_GUARANTEED_PERF]		= CPPC_REG_MSR(IA32_HWP_CAPABILITIES, 8, 8);
	config->entries[CPPC_DESIRED_PERF]		= CPPC_REG_MSR(IA32_HWP_REQUEST, 16, 8);
	config->entries[CPPC_MIN_PERF]			= CPPC_REG_MSR(IA32_HWP_REQUEST, 0, 8);
	config->entries[CPPC_MAX_PERF]			= CPPC_REG_MSR(IA32_HWP_REQUEST, 8, 8);
	config->entries[CPPC_PERF_REDUCE_TOLERANCE]	= CPPC_UNSUPPORTED;
	config->entries[CPPC_TIME_WINDOW]		= CPPC_UNSUPPORTED;
	config->entries[CPPC_COUNTER_WRAP]		= CPPC_UNSUPPORTED;
	config->entries[CPPC_REF_PERF_COUNTER]		= CPPC_REG_MSR(IA32_MPERF, 0, 64);
	config->entries[CPPC_DELIVERED_PERF_COUNTER]	= CPPC_REG_MSR(IA32_APERF, 0, 64);
	config->entries[CPPC_PERF_LIMITED]		= CPPC_REG_MSR(IA32_HWP_STATUS, 2, 1);
	config->entries[CPPC_ENABLE]			= CPPC_REG_MSR(IA32_PM_ENABLE, 0, 1);

	if (version < 2)
		return;

	config->entries[CPPC_AUTO_SELECT]		= CPPC_DWORD(1);
	config->entries[CPPC_AUTO_ACTIVITY_WINDOW]	= CPPC_REG_MSR(IA32_HWP_REQUEST, 32, 10);
	config->entries[CPPC_PERF_PREF]			= CPPC_REG_MSR(IA32_HWP_REQUEST, 24, 8);
	config->entries[CPPC_REF_PERF]			= CPPC_UNSUPPORTED;

	if (version < 3)
		return;

	config->entries[CPPC_LOWEST_FREQ]		= CPPC_UNSUPPORTED;
	config->entries[CPPC_NOMINAL_FREQ]		= CPPC_UNSUPPORTED;
}

void set_aesni_lock(void)
{
	msr_t msr;

	if (!CONFIG(SET_MSR_AESNI_LOCK_BIT))
		return;

	if (!(cpu_get_feature_flags_ecx() & CPUID_AES))
		return;

	/* Only run once per core as specified in the MSR datasheet */
	if (intel_ht_sibling())
		return;

	msr = rdmsr(MSR_FEATURE_CONFIG);
	if (msr.lo & AESNI_LOCK)
		return;

	msr_set(MSR_FEATURE_CONFIG, AESNI_LOCK);
}

void enable_lapic_tpr(void)
{
	msr_unset(MSR_PIC_MSG_CONTROL, TPR_UPDATES_DISABLE);
}

void configure_dca_cap(void)
{
	if (cpu_get_feature_flags_ecx() & CPUID_DCA)
		msr_set(IA32_PLATFORM_DCA_CAP, DCA_TYPE0_EN);
}

void set_energy_perf_bias(u8 policy)
{
	u8 epb = policy & ENERGY_POLICY_MASK;

	if (!(cpuid_ecx(6) & CPUID_6_ECX_EPB))
		return;

	msr_unset_and_set(IA32_ENERGY_PERF_BIAS, ENERGY_POLICY_MASK, epb);
	printk(BIOS_DEBUG, "cpu: energy policy set to %u\n", epb);
}

/*
 * Check energy performance preference and HWP capabilities from Thermal and
 * Power Management Leaf CPUID
 */
bool check_energy_perf_cap(void)
{
	const u32 cap = cpuid_eax(CPUID_LEAF_PM);
	if (!(cap & CPUID_6_ENGERY_PERF_PREF))
		return false;
	if (!(cap & CPUID_6_HWP))
		return false;
	return true;
}

/*
 * Instructs the CPU to use EPP hints. This means that any energy policies set
 * up in `set_energy_perf_bias` will be ignored afterwards.
 */
void enable_energy_perf_pref(void)
{
	msr_t msr = rdmsr(IA32_PM_ENABLE);
	if (!(msr.lo & HWP_ENABLE)) {
		/* Package-scoped MSR */
		printk(BIOS_DEBUG, "HWP_ENABLE: energy-perf preference in favor of energy-perf bias\n");
		msr_set(IA32_PM_ENABLE, HWP_ENABLE);
	}
}

/*
 * Set the IA32_HWP_REQUEST Energy-Performance Preference bits on the logical
 * thread. 0 is a hint to the HWP to prefer performance, and 255 is a hint to
 * prefer energy efficiency.
 * This function needs to be called when HWP_ENABLE is set.
*/
void set_energy_perf_pref(u8 pref)
{
	msr_unset_and_set(IA32_HWP_REQUEST, IA32_HWP_REQUEST_EPP_MASK,
		pref << IA32_HWP_REQUEST_EPP_SHIFT);
}
