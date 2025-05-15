/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/debug.h>
#include <cpu/intel/common/common.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/topology.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/mp_init.h>
#include <intelpch/lockdown.h>
#include <soc/msr.h>
#include <soc/pm.h>
#include <soc/smmrelocate.h>
#include <soc/util.h>

#include "chip.h"

static const void *microcode_patch;

static const config_t *chip_config = NULL;

bool cpu_soc_is_in_untrusted_mode(void)
{
	return false;
}

void cpu_soc_bios_done(void)
{
}

static void xeon_configure_mca(void)
{
	msr_t msr;
	struct cpuid_result cpuid_regs;

	/*
	 * Check feature flag in CPUID.(EAX=1):EDX[7]==1  MCE
	 *                   and CPUID.(EAX=1):EDX[14]==1 MCA
	 */
	cpuid_regs = cpuid(1);
	if ((cpuid_regs.edx & (1 << 7 | 1 << 14)) != (1 << 7 | 1 << 14))
		return;

	msr = rdmsr(IA32_MCG_CAP);
	if (msr.lo & IA32_MCG_CAP_CTL_P_MASK) {
		/* Enable all error logging */
		msr.lo = msr.hi = 0xffffffff;
		wrmsr(IA32_MCG_CTL, msr);
	}

	mca_configure();
}

/*
 * On server platforms the FIT mechanism only updates the microcode on
 * the BSP. Loading MCU on AP in parallel seems to fail in 10% of the cases
 * so do it serialized.
 */
void get_microcode_info(const void **microcode, int *parallel)
{
	*microcode = intel_microcode_find();
	*parallel = 0;
}

static void each_cpu_init(struct device *cpu)
{
	msr_t msr;

	printk(BIOS_SPEW, "%s dev: %s, cpu: %lu, apic_id: 0x%x, package_id: 0x%x\n",
	       __func__, dev_path(cpu), cpu_index(), cpu->path.apic.apic_id,
	       cpu->path.apic.package_id);

	assert (cpu->path.apic.node_id < CONFIG_MAX_SOCKET);

	/*
	 * Enable PWR_PERF_PLTFRM_OVR and PROCHOT_LOCK.
	 * The value set by FSP is 20_005f, we set it to 1a_00a4_005b.
	 */
	msr = rdmsr(MSR_POWER_CTL);
	msr.lo |= (0x16 << RESERVED1_SHIFT) | PWR_PERF_PLTFRM_OVR | PROCHOT_LOCK;
	msr.hi = 0x1a;
	wrmsr(MSR_POWER_CTL, msr);

	/* Set static, idle, dynamic load line impedance */
	msr = rdmsr(MSR_VR_MISC_CONFIG);
	msr.lo = 0x1a1a1a;
	wrmsr(MSR_VR_MISC_CONFIG, msr);

	/* Set current limitation */
	msr = rdmsr(MSR_VR_CURRENT_CONFIG);
	msr.lo = 0x1130;
	msr.lo |= CURRENT_LIMIT_LOCK;
	wrmsr(MSR_VR_CURRENT_CONFIG, msr);

	/* Set Turbo Ratio Limits */
	if (chip_config->turbo_ratio_limit) {
		msr.lo = chip_config->turbo_ratio_limit & 0xffffffff;
		msr.hi = (chip_config->turbo_ratio_limit >> 32) & 0xffffffff;
		wrmsr(MSR_TURBO_RATIO_LIMIT, msr);
	}

	/* Set Turbo Ratio Limit Cores */
	if (chip_config->turbo_ratio_limit_cores) {
		msr.lo = chip_config->turbo_ratio_limit_cores & 0xffffffff;
		msr.hi = (chip_config->turbo_ratio_limit_cores >> 32) & 0xffffffff;
		wrmsr(MSR_TURBO_RATIO_LIMIT_CORES, msr);
	}

	/* Set energy policy */
	msr = rdmsr(MSR_ENERGY_PERF_BIAS_CONFIG);
	msr.lo = 0x178fa038;
	wrmsr(MSR_ENERGY_PERF_BIAS_CONFIG, msr);

	msr.hi = 0x158d20;
	msr.lo = 0x00158af0;
	wrmsr(PACKAGE_RAPL_LIMIT, msr);

	/*
	 * Set HWP base feature, EPP reg enumeration, lock thermal and msr
	 * This is package level MSR. Need to check if it updates correctly on
	 * multi-socket platform.
	 */
	msr = rdmsr(MSR_MISC_PWR_MGMT);
	if (!(msr.lo & LOCK_MISC_PWR_MGMT_MSR)) { /* if already locked skip update */
		msr.lo = (HWP_ENUM_ENABLE | HWP_EPP_ENUM_ENABLE | LOCK_MISC_PWR_MGMT_MSR
			  | LOCK_THERM_INT);
		wrmsr(MSR_MISC_PWR_MGMT, msr);
	}

	/* Enable Fast Strings */
	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= FAST_STRINGS_ENABLE_BIT;
	wrmsr(IA32_MISC_ENABLE, msr);
	/* Enable Turbo */
	enable_turbo();

	/* Enable speed step. */
	if (get_turbo_state() == TURBO_ENABLED) {
		msr = rdmsr(IA32_MISC_ENABLE);
		msr.lo |= SPEED_STEP_ENABLE_BIT;
		wrmsr(IA32_MISC_ENABLE, msr);
	}

	/* Lock the supported Cstates */
	msr = rdmsr(MSR_PKG_CST_CONFIG_CONTROL);
	msr.lo |= CST_CFG_LOCK_MASK;
	wrmsr(MSR_PKG_CST_CONFIG_CONTROL, msr);

	/* Disable all writes to overclocking limits MSR */
	msr = rdmsr(MSR_FLEX_RATIO);
	msr.lo |= MSR_FLEX_RATIO_OC_LOCK;
	wrmsr(MSR_FLEX_RATIO, msr);

	/* Lock Power Plane Limit MSR */
	msr = rdmsr(MSR_DRAM_PLANE_POWER_LIMIT);
	msr.hi |= MSR_HI_PP_PWR_LIM_LOCK;
	wrmsr(MSR_DRAM_PLANE_POWER_LIMIT, msr);

	/* Clear out pending MCEs */
	xeon_configure_mca();

	/* Enable Vmx */
	// set_vmx_and_lock();
	/* only lock. let vmx enable by FSP  */
	set_feature_ctrl_lock();
}

static struct device_operations cpu_dev_ops = {
	.init = each_cpu_init,
};

static const struct cpu_device_id cpu_table[] = {
	{X86_VENDOR_INTEL, CPUID_SAPPHIRERAPIDS_SP_D, CPUID_EXACT_MATCH_MASK},
	{X86_VENDOR_INTEL, CPUID_SAPPHIRERAPIDS_SP_E0, CPUID_EXACT_MATCH_MASK},
	{X86_VENDOR_INTEL, CPUID_SAPPHIRERAPIDS_SP_E2, CPUID_EXACT_MATCH_MASK},
	{X86_VENDOR_INTEL, CPUID_SAPPHIRERAPIDS_SP_E3, CPUID_EXACT_MATCH_MASK},
	{X86_VENDOR_INTEL, CPUID_SAPPHIRERAPIDS_SP_E4, CPUID_EXACT_MATCH_MASK},
	{X86_VENDOR_INTEL, CPUID_SAPPHIRERAPIDS_SP_Ex, CPUID_EXACT_MATCH_MASK},
	{X86_VENDOR_INTEL, CPUID_EMERALDRAPIDS, CPUID_EXACT_MATCH_MASK},
	CPU_TABLE_END
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
	       ((perf_ctl.lo >> 8) & 0xff) * CONFIG_CPU_BCLK_MHZ);
	FUNC_EXIT();
}

/*
 * Do essential initialization tasks before APs can be fired up
 */
static void pre_mp_init(void)
{
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

static void post_mp_init(void)
{
	/* Set Max Ratio */
	set_max_turbo_freq();

	if (CONFIG(HAVE_SMI_HANDLER)) {
		global_smi_enable();
		if (get_lockdown_config() == CHIPSET_LOCKDOWN_COREBOOT)
			pmc_lock_smi();
	}
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_platform_thread_count,
#if CONFIG(HAVE_SMI_HANDLER)
	.get_smm_info = get_smm_info,
	.pre_mp_smm_init = smm_southbridge_clear_state,
	.relocation_handler = smm_relocation_handler,
#endif
	.get_microcode_info = get_microcode_info,
	.post_mp_init = post_mp_init,
};

void mp_init_cpus(struct bus *bus)
{
	/*
	 * chip_config is used in cpu device callback. Other than cpu 0,
	 * rest of the CPU devices do not have chip_info updated.
	 */
	chip_config = bus->dev->chip_info;

	microcode_patch = intel_microcode_find();
	intel_microcode_load_unlocked(microcode_patch);

	enum cb_err ret = mp_init_with_smm(bus, &mp_ops);
	if (ret != CB_SUCCESS)
		printk(BIOS_ERR, "MP initialization failure %d.\n", ret);
}
