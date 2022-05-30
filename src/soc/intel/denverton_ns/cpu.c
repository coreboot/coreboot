/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/em64t100_save_state.h>
#include <cpu/intel/turbo.h>
#include <cpu/intel/common/common.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/cpulib.h>
#include <lib.h>
#include <soc/msr.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/smm.h>
#include <soc/soc_util.h>
#include <types.h>

bool cpu_soc_is_in_untrusted_mode(void)
{
	msr_t msr;

	msr = rdmsr(MSR_POWER_MISC);
	return !!(msr.lo & ENABLE_IA_UNTRUSTED);
}

void cpu_soc_bios_done(void)
{
	msr_t msr;

	msr = rdmsr(MSR_POWER_MISC);
	msr.lo |= ENABLE_IA_UNTRUSTED;
	wrmsr(MSR_POWER_MISC, msr);
}

static struct smm_relocation_attrs relo_attrs;

static void dnv_configure_mca(void)
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

	/* TODO install a fallback MC handler for each core in case OS does
	   not provide one. Is it really needed? */

	/* Enable the machine check exception */
	write_cr4(read_cr4() | CR4_MCE);
}

static void configure_thermal_core(void)
{
	msr_t msr;

	/* Disable Thermal interrupts */
	msr.lo = 0;
	msr.hi = 0;
	wrmsr(IA32_THERM_INTERRUPT, msr);
	wrmsr(IA32_PACKAGE_THERM_INTERRUPT, msr);

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= THERMAL_MONITOR_ENABLE_BIT;	/* TM1/TM2/EMTTM enable */
	wrmsr(IA32_MISC_ENABLE, msr);
}

static void denverton_core_init(struct device *cpu)
{
	msr_t msr;

	printk(BIOS_DEBUG, "Init Denverton-NS SoC cores.\n");

	/* Clear out pending MCEs */
	dnv_configure_mca();

	/* Configure Thermal Sensors */
	configure_thermal_core();

	/* Enable Fast Strings */
	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= FAST_STRINGS_ENABLE_BIT;
	wrmsr(IA32_MISC_ENABLE, msr);

	set_aesni_lock();

	/* Enable Turbo */
	enable_turbo();

	/* Enable speed step. Always ON.*/
	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= SPEED_STEP_ENABLE_BIT;
	wrmsr(IA32_MISC_ENABLE, msr);

	enable_pm_timer_emulation();
}

static struct device_operations cpu_dev_ops = {
	.init = denverton_core_init,
};

static const struct cpu_device_id cpu_table[] = {
	{X86_VENDOR_INTEL,
	 CPUID_DENVERTON_A0_A1},		/* Denverton-NS A0/A1 CPUID */
	{X86_VENDOR_INTEL, CPUID_DENVERTON_B0}, /* Denverton-NS B0 CPUID */
	{0, 0},
};

static const struct cpu_driver driver __cpu_driver = {
	.ops = &cpu_dev_ops,
	.id_table = cpu_table,
};

/*
 * MP and SMM loading initialization.
 */

static void relocation_handler(int cpu, uintptr_t curr_smbase,
			       uintptr_t staggered_smbase)
{
	msr_t smrr;
	em64t100_smm_state_save_area_t *smm_state;
	(void)cpu;

	/* Set up SMRR. */
	smrr.lo = relo_attrs.smrr_base;
	smrr.hi = 0;
	wrmsr(IA32_SMRR_PHYS_BASE, smrr);
	smrr.lo = relo_attrs.smrr_mask;
	smrr.hi = 0;
	wrmsr(IA32_SMRR_PHYS_MASK, smrr);
	smm_state = (void *)(SMM_EM64T100_SAVE_STATE_OFFSET + curr_smbase);
	smm_state->smbase = staggered_smbase;
}

static void get_smm_info(uintptr_t *perm_smbase, size_t *perm_smsize,
			 size_t *smm_save_state_size)
{
	uintptr_t smm_base;
	size_t smm_size;
	uintptr_t handler_base;
	size_t handler_size;

	/* All range registers are aligned to 4KiB */
	const uint32_t rmask = ~((1 << 12) - 1);

	/* Initialize global tracking state. */
	smm_region(&smm_base, &smm_size);
	smm_subregion(SMM_SUBREGION_HANDLER, &handler_base, &handler_size);

	relo_attrs.smbase = smm_base;
	relo_attrs.smrr_base = relo_attrs.smbase | MTRR_TYPE_WRBACK;
	relo_attrs.smrr_mask = ~(smm_size - 1) & rmask;
	relo_attrs.smrr_mask |= MTRR_PHYS_MASK_VALID;

	*perm_smbase = handler_base;
	*perm_smsize = handler_size;
	*smm_save_state_size = sizeof(em64t100_smm_state_save_area_t);
}

static unsigned int detect_num_cpus_via_cpuid(void)
{
	unsigned int ecx = 0;

	while (1) {
		const struct cpuid_result leaf_b = cpuid_ext(0xb, ecx);

		/* Processor doesn't have hyperthreading so just determine the
		   number of cores from level type (ecx[15:8] == 2). */
		if ((leaf_b.ecx >> 8 & 0xff) == 2)
			return leaf_b.ebx & 0xffff;

		ecx++;
	}
}

/* Assumes that FSP has already programmed the cores disabled register */
static unsigned int detect_num_cpus_via_mch(void)
{
	/* Get Masks for Total Existing SOC Cores and Core Disable Mask */
	const u32 core_exists_mask = MMIO32(DEFAULT_MCHBAR + MCH_BAR_CORE_EXISTS_MASK);
	const u32 core_disable_mask = MMIO32(DEFAULT_MCHBAR + MCH_BAR_CORE_DISABLE_MASK);
	const u32 active_cores_mask = ~core_disable_mask & core_exists_mask;

	/* Calculate Number of Active Cores */
	const unsigned int active_cores = popcnt(active_cores_mask);
	const unsigned int total_cores = popcnt(core_exists_mask);

	printk(BIOS_DEBUG, "Number of Active Cores: %u of %u total.\n",
	       active_cores, total_cores);

	return active_cores;
}

/* Find CPU topology */
int get_cpu_count(void)
{
	unsigned int num_cpus = detect_num_cpus_via_mch();

	if (num_cpus == 0 || num_cpus > CONFIG_MAX_CPUS) {
		num_cpus = detect_num_cpus_via_cpuid();
		printk(BIOS_DEBUG, "Number of Cores (CPUID): %u.\n", num_cpus);
	}
	return num_cpus;
}

static void set_max_turbo_freq(void)
{
	msr_t msr, perf_ctl;

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
}

/*
 * Do essential initialization tasks before APs can be fired up
 *
 * 1. Prevent race condition in MTRR solution. Enable MTRRs on the BSP. This
 * creates the MTRR solution that the APs will use. Otherwise APs will try to
 * apply the incomplete solution as the BSP is calculating it.
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

	/*
	 * Now that all APs have been relocated as well as the BSP let SMIs
	 * start flowing.
	 */
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
	.get_cpu_count = get_cpu_count,
	.get_smm_info = get_smm_info,
	.pre_mp_smm_init = smm_southbridge_clear_state,
	.relocation_handler = relocation_handler,
	.post_mp_init = post_mp_init,
};

void mp_init_cpus(struct bus *cpu_bus)
{
	/* Clear for take-off */
	/* TODO: Handle mp_init_with_smm failure? */
	mp_init_with_smm(cpu_bus, &mp_ops);
}
