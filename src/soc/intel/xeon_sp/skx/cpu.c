/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <console/debug.h>
#include <intelblocks/cpulib.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/turbo.h>
#include <soc/msr.h>
#include <soc/cpu.h>
#include <soc/soc_util.h>
#include <soc/smmrelocate.h>
#include <soc/util.h>
#include <assert.h>
#include "chip.h"
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/em64t101_save_state.h>
#include <types.h>

static const config_t *chip_config = NULL;

bool cpu_soc_is_in_untrusted_mode(void)
{
	/* IA_UNTRUSTED_MODE is not supported in Skylake */
	return false;
}

void cpu_soc_bios_done(void)
{
	/* IA_UNTRUSTED_MODE is not supported in Skylake */
}

static void xeon_configure_mca(void)
{
	msr_t msr;
	struct cpuid_result cpuid_regs;

	/* Check feature flag in CPUID.(EAX=1):EDX[7]==1  MCE
	 *                   and CPUID.(EAX=1):EDX[14]==1 MCA*/
	cpuid_regs = cpuid(1);
	if ((cpuid_regs.edx & (1<<7 | 1<<14)) != (1<<7 | 1<<14))
		return;

	msr = rdmsr(IA32_MCG_CAP);
	if (msr.lo & IA32_MCG_CAP_CTL_P_MASK) {
		/* Enable all error logging */
		msr.lo = msr.hi = 0xffffffff;
		wrmsr(IA32_MCG_CTL, msr);
	}

	/* TODO(adurbin): This should only be done on a cold boot. Also, some
	   of these banks are core vs package scope. For now every CPU clears
	   every bank. */
	mca_configure();
}

static void xeon_sp_core_init(struct device *cpu)
{
	msr_t msr;

	printk(BIOS_INFO, "%s dev: %s, cpu: %d, apic_id: 0x%x\n",
		__func__, dev_path(cpu), cpu_index(), cpu->path.apic.apic_id);
	assert(chip_config);

	/* set MSR_PKG_CST_CONFIG_CONTROL - scope per core*/
	msr.hi = 0;
	msr.lo = (PKG_CSTATE_NO_LIMIT | CFG_LOCK_ENABLE);
	wrmsr(MSR_PKG_CST_CONFIG_CONTROL, msr);

	/* Enable Energy Perf Bias Access, Dynamic switching and lock MSR */
	msr = rdmsr(MSR_POWER_CTL);
	msr.lo |= (ENERGY_PERF_BIAS_ACCESS_ENABLE | PWR_PERF_TUNING_DYN_SWITCHING_ENABLE
		| PROCHOT_LOCK_ENABLE);
	wrmsr(MSR_POWER_CTL, msr);

	/* Set P-State ratio */
	msr = rdmsr(MSR_IA32_PERF_CTRL);
	msr.lo &= ~PSTATE_REQ_MASK;
	msr.lo |= (chip_config->pstate_req_ratio << PSTATE_REQ_SHIFT);
	wrmsr(MSR_IA32_PERF_CTRL, msr);

	/*
	 * Set HWP base feature, EPP reg enumeration, lock thermal and msr
	 * TODO: Set LOCK_MISC_PWR_MGMT_MSR, Unexpected Exception if you
	 * lock & issue wrmsr on every thread
	 * This is package level MSR. Need to check if it updates correctly on
	 * multi-socket platform.
	 */
	msr = rdmsr(MSR_MISC_PWR_MGMT);
	if (!(msr.lo & LOCK_MISC_PWR_MGMT_MSR)) { /* if already locked skip update */
		msr.lo = (HWP_ENUM_ENABLE | HWP_EPP_ENUM_ENABLE | LOCK_MISC_PWR_MGMT_MSR |
			LOCK_THERM_INT);
		wrmsr(MSR_MISC_PWR_MGMT, msr);
	}

	/* TODO MSR_VR_MISC_CONFIG */

	/* Set current limit lock */
	msr = rdmsr(MSR_VR_CURRENT_CONFIG);
	msr.lo |= CURRENT_LIMIT_LOCK;
	wrmsr(MSR_VR_CURRENT_CONFIG, msr);

	/* Set Turbo Ratio Limits */
	msr.lo = chip_config->turbo_ratio_limit & 0xffffffff;
	msr.hi = (chip_config->turbo_ratio_limit >> 32) & 0xffffffff;
	wrmsr(MSR_TURBO_RATIO_LIMIT, msr);

	/* Set Turbo Ratio Limit Cores */
	msr.lo = chip_config->turbo_ratio_limit_cores & 0xffffffff;
	msr.hi = (chip_config->turbo_ratio_limit_cores >> 32) & 0xffffffff;
	wrmsr(MSR_TURBO_RATIO_LIMIT_CORES, msr);

	/* set Turbo Activation ratio */
	msr.hi = 0;
	msr = rdmsr(MSR_TURBO_ACTIVATION_RATIO);
	msr.lo |= MAX_NON_TURBO_RATIO;
	wrmsr(MSR_TURBO_ACTIVATION_RATIO, msr);

	/* Enable Fast Strings */
	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= FAST_STRINGS_ENABLE_BIT;
	wrmsr(IA32_MISC_ENABLE, msr);

	/* Set energy policy */
	msr_t msr1 = rdmsr(MSR_ENERGY_PERF_BIAS_CONFIG);
	msr.lo = (msr1.lo & EPB_ENERGY_POLICY_MASK) >> EPB_ENERGY_POLICY_SHIFT;
	msr.hi = 0;
	wrmsr(MSR_IA32_ENERGY_PERF_BIAS, msr);

	/* Enable Turbo */
	enable_turbo();

	/* Enable speed step. */
	if (get_turbo_state() == TURBO_ENABLED) {
		msr = rdmsr(IA32_MISC_ENABLE);
		msr.lo |= SPEED_STEP_ENABLE_BIT;
		wrmsr(IA32_MISC_ENABLE, msr);
	}

	/* Clear out pending MCEs */
	xeon_configure_mca();
}

static struct device_operations cpu_dev_ops = {
	.init = xeon_sp_core_init,
};

static const struct cpu_device_id cpu_table[] = {
	/* Skylake-SP A0/A1 CPUID 0x506f0*/
	{X86_VENDOR_INTEL, CPUID_SKYLAKE_SP_A0_A1},
	/* Skylake-SP B0 CPUID 0x506f1*/
	{X86_VENDOR_INTEL, CPUID_SKYLAKE_SP_B0},
	/* Skylake-SP 4 CPUID 0x50654*/
	{X86_VENDOR_INTEL, CPUID_SKYLAKE_SP_4},
	{0, 0},
};

static const struct cpu_driver driver __cpu_driver = {
	.ops = &cpu_dev_ops,
	.id_table = cpu_table,
};

static void set_max_turbo_freq(void)
{
	msr_t msr, perf_ctl;

	FUNC_ENTER();
	perf_ctl.hi = 0;

	/* Check for configurable TDP option */
	if (get_turbo_state() == TURBO_ENABLED) {
		msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
		perf_ctl.lo = (msr.lo & 0xff) << 8;
	} else if (cpu_config_tdp_levels()) {
		/* Set to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		perf_ctl.lo = (msr.lo & 0xff) << 8;
	} else {
		/* Platform Info bits 15:8 give max ratio */
		msr = rdmsr(MSR_PLATFORM_INFO);
		perf_ctl.lo = msr.lo & 0xff00;
	}
	wrmsr(IA32_PERF_CTL, perf_ctl);

	printk(BIOS_DEBUG, "cpu: frequency set to %d\n",
	       ((perf_ctl.lo >> 8) & 0xff) * CPU_BCLK);
	FUNC_EXIT();
}

/*
 * Do essential initialization tasks before APs can be fired up
 *
 * Prevent race condition in MTRR solution. Enable MTRRs on the BSP. This
 * creates the MTRR solution that the APs will use. Otherwise APs will try to
 * apply the incomplete solution as the BSP is calculating it.
 */
static void pre_mp_init(void)
{
	printk(BIOS_DEBUG, "%s: entry\n", __func__);

	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

static void post_mp_init(void)
{
	/* Set Max Ratio */
	set_max_turbo_freq();

	if (CONFIG(HAVE_SMI_HANDLER))
		global_smi_enable();
}

/*
 * CPU initialization recipe
 *
 * Note that no microcode update is passed to the init function. CSE updates
 * the microcode on all cores before releasing them from reset. That means that
 * the BSP and all APs will come up with the same microcode revision.
 */
static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_platform_thread_count,
	.get_smm_info = get_smm_info,
	.pre_mp_smm_init = smm_southbridge_clear_state,
	.relocation_handler = smm_relocation_handler,
	.post_mp_init = post_mp_init,
};

void xeon_sp_init_cpus(struct device *dev)
{
	FUNC_ENTER();

	/*
	 * This gets used in cpu device callback. Other than cpu 0,
	 * rest of the CPU devices do not have
	 * chip_info updated. Global chip_config is used as workaround
	 */
	chip_config = dev->chip_info;

	config_reset_cpl3_csrs();

	/* calls src/cpu/x86/mp_init.c */
	/* TODO: Handle mp_init_with_smm failure? */
	mp_init_with_smm(dev->link_list, &mp_ops);

	/* update numa domain for all cpu devices */
	xeonsp_init_cpu_config();

	FUNC_EXIT();
}
