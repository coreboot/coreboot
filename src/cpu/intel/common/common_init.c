/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <cpu/intel/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/msr.h>
#include "common.h"

#define  CPUID_6_ECX_EPB	(1 << 3)

void set_vmx_and_lock(void)
{
	set_feature_ctrl_vmx();
	set_feature_ctrl_lock();
}

void set_feature_ctrl_vmx(void)
{
	msr_t msr;
	uint32_t feature_flag;
	int enable = CONFIG(ENABLE_VMX);

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
	acpi_addr_t msr = {
		.space_id   = ACPI_ADDRESS_SPACE_FIXED,
		.bit_width  = 8,
		.bit_offset = 0,
		.access_size = ACPI_ACCESS_SIZE_QWORD_ACCESS,
		.addrl      = 0,
		.addrh      = 0,
	};
	static const acpi_addr_t unsupported = {
		.space_id   = ACPI_ADDRESS_SPACE_MEMORY,
		.bit_width  = 0,
		.bit_offset = 0,
		.access_size = ACPI_ACCESS_SIZE_UNDEFINED,
		.addrl      = 0,
		.addrh      = 0,
	};

	config->version = version;

	msr.addrl = IA32_HWP_CAPABILITIES;

	/*
	 * Highest Performance:
	 * ResourceTemplate(){Register(FFixedHW, 0x08, 0x00, 0x771, 0x04,)},
	 */
	config->regs[CPPC_HIGHEST_PERF] = msr;

	/*
	 * Nominal Performance -> Guaranteed Performance:
	 * ResourceTemplate(){Register(FFixedHW, 0x08, 0x08, 0x771, 0x04,)},
	 */
	msr.bit_offset = 8;
	config->regs[CPPC_NOMINAL_PERF] = msr;

	/*
	 * Lowest Nonlinear Performance -> Most Efficient Performance:
	 * ResourceTemplate(){Register(FFixedHW, 0x08, 0x10, 0x771, 0x04,)},
	 */
	msr.bit_offset = 16;
	config->regs[CPPC_LOWEST_NONL_PERF] = msr;

	/*
	 * Lowest Performance:
	 * ResourceTemplate(){Register(FFixedHW, 0x08, 0x18, 0x771, 0x04,)},
	 */
	msr.bit_offset = 24;
	config->regs[CPPC_LOWEST_PERF] = msr;

	/*
	 * Guaranteed Performance Register:
	 * ResourceTemplate(){Register(FFixedHW, 0x08, 0x08, 0x771, 0x04,)},
	 */
	msr.bit_offset = 8;
	config->regs[CPPC_GUARANTEED_PERF] = msr;

	msr.addrl = IA32_HWP_REQUEST;

	/*
	 * Desired Performance Register:
	 * ResourceTemplate(){Register(FFixedHW, 0x08, 0x10, 0x774, 0x04,)},
	 */
	msr.bit_offset = 16;
	config->regs[CPPC_DESIRED_PERF] = msr;

	/*
	 * Minimum Performance Register:
	 * ResourceTemplate(){Register(FFixedHW, 0x08, 0x00, 0x774, 0x04,)},
	 */
	msr.bit_offset = 0;
	config->regs[CPPC_MIN_PERF] = msr;

	/*
	 * Maximum Performance Register:
	 * ResourceTemplate(){Register(FFixedHW, 0x08, 0x08, 0x774, 0x04,)},
	 */
	msr.bit_offset = 8;
	config->regs[CPPC_MAX_PERF] = msr;

	/*
	 * Performance Reduction Tolerance Register:
	 * ResourceTemplate(){Register(SystemMemory, 0x00, 0x00, 0x0,,)},
	 */
	config->regs[CPPC_PERF_REDUCE_TOLERANCE] = unsupported;

	/*
	 * Time Window Register:
	 * ResourceTemplate(){Register(SystemMemory, 0x00, 0x00, 0x0,,)},
	 */
	config->regs[CPPC_TIME_WINDOW] = unsupported;

	/*
	 * Counter Wraparound Time:
	 * ResourceTemplate(){Register(SystemMemory, 0x00, 0x00, 0x0,,)},
	 */
	config->regs[CPPC_COUNTER_WRAP] = unsupported;

	msr.addrl = IA32_MPERF;

	/*
	 * Reference Performance Counter Register:
	 * ResourceTemplate(){Register(FFixedHW, 0x40, 0x00, 0x0E7, 0x04,)},
	 */
	msr.bit_width = 64;
	msr.bit_offset = 0;
	config->regs[CPPC_REF_PERF_COUNTER] = msr;

	msr.addrl = IA32_APERF;

	/*
	 * Delivered Performance Counter Register:
	 * ResourceTemplate(){Register(FFixedHW, 0x40, 0x00, 0x0E8, 0x04,)},
	 */
	config->regs[CPPC_DELIVERED_PERF_COUNTER] = msr;

	msr.addrl = IA32_HWP_STATUS;

	/*
	 * Performance Limited Register:
	 * ResourceTemplate(){Register(FFixedHW, 0x01, 0x02, 0x777, 0x04,)},
	 */
	msr.bit_width = 1;
	msr.bit_offset = 2;
	config->regs[CPPC_PERF_LIMITED] = msr;

	msr.addrl = IA32_PM_ENABLE;

	/*
	 * CPPC Enable Register:
	 * ResourceTemplate(){Register(FFixedHW, 0x01, 0x00, 0x770, 0x04,)},
	 */
	msr.bit_offset = 0;
	config->regs[CPPC_ENABLE] = msr;

	if (version >= 2) {
		/* Autonomous Selection Enable is populated below */

		/* Autonomous Activity Window Register */
		config->regs[CPPC_AUTO_ACTIVITY_WINDOW] = unsupported;

		/* Energy Performance Preference Register */
		config->regs[CPPC_PERF_PREF] = unsupported;

		/* Reference Performance */
		config->regs[CPPC_REF_PERF] = unsupported;

		if (version >= 3) {
			/* Lowest Frequency */
			config->regs[CPPC_LOWEST_FREQ] = unsupported;
			/* Nominal Frequency */
			config->regs[CPPC_NOMINAL_FREQ] = unsupported;
		}

		/*
		 * Autonomous Selection Enable = 1
		 * This field is actually the first addition in version 2 but
		 * it's so unlike the others I'm populating it last.
		 */
		msr.space_id    = ACPI_ADDRESS_SPACE_MEMORY;
		msr.bit_width   = 32;
		msr.bit_offset  = 0;
		msr.access_size = ACPI_ACCESS_SIZE_UNDEFINED;
		msr.addrl       = 1;
		config->regs[CPPC_AUTO_SELECT] = msr;
	}
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
