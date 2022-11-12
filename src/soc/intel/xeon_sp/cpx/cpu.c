/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <assert.h>
#include <console/console.h>
#include <console/debug.h>
#include <cpu/cpu.h>
#include <cpu/intel/common/common.h>
#include <cpu/intel/em64t101_save_state.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/mtrr.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/mp_init.h>
#include <intelpch/lockdown.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/smmrelocate.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <types.h>

#include "chip.h"

static const void *microcode_patch;

static const config_t *chip_config = NULL;

bool cpu_soc_is_in_untrusted_mode(void)
{
	/* IA_UNTRUSTED_MODE is not supported in Cooper Lake */
	return false;
}

void cpu_soc_bios_done(void)
{
	/* IA_UNTRUSTED_MODE is not supported in Cooper Lake */
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

	printk(BIOS_SPEW, "%s dev: %s, cpu: %lu, apic_id: 0x%x\n",
		__func__, dev_path(cpu), cpu_index(), cpu->path.apic.apic_id);

	/*
	 * Set HWP base feature, EPP reg enumeration, lock thermal and msr
	 * This is package level MSR. Need to check if it updates correctly on
	 * multi-socket platform.
	 */
	msr = rdmsr(MSR_MISC_PWR_MGMT);
	if (!(msr.lo & LOCK_MISC_PWR_MGMT_MSR)) { /* if already locked skip update */
		msr.lo = (HWP_ENUM_ENABLE | HWP_EPP_ENUM_ENABLE | LOCK_MISC_PWR_MGMT_MSR |
			LOCK_THERM_INT);
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

	/* Clear out pending MCEs */
	xeon_configure_mca();

	/* Enable Vmx */
	set_vmx_and_lock();
	set_aesni_lock();

	/* The MSRs and CSRS have the same register layout. Use the CSRS bit definitions
	   Lock Turbo. Did FSP-S set this up??? */
	msr = rdmsr(MSR_TURBO_ACTIVATION_RATIO);
	msr.lo |= (TURBO_ACTIVATION_RATIO_LOCK);
	wrmsr(MSR_TURBO_ACTIVATION_RATIO, msr);
}

static struct device_operations cpu_dev_ops = {
	.init = each_cpu_init,
};

static const struct cpu_device_id cpu_table[] = {
	{X86_VENDOR_INTEL, CPUID_COOPERLAKE_SP_A0},
	{X86_VENDOR_INTEL, CPUID_COOPERLAKE_SP_A1},
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

static int get_thread_count(void)
{
	unsigned int num_phys = 0, num_virts = 0;

	cpu_read_topology(&num_phys, &num_virts);
	printk(BIOS_SPEW, "Detected %u cores and %u threads\n", num_phys, num_virts);
	/*
	 * Currently we do not know a way to figure out how many CPUs we have total
	 * on multi-socketed. So we pretend all sockets are populated with CPUs with
	 * same thread/core fusing.
	 * TODO: properly figure out number of active sockets OR refactor MPinit code
	 * to remove requirements of having to know total number of CPUs in advance.
	 */
	return num_virts * CONFIG_MAX_SOCKET;
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
	.get_cpu_count = get_thread_count,
	.get_smm_info = get_smm_info,
	.pre_mp_smm_init = smm_southbridge_clear_state,
	.relocation_handler = smm_relocation_handler,
	.get_microcode_info = get_microcode_info,
	.post_mp_init = post_mp_init,
};

void cpx_init_cpus(struct device *dev)
{
	microcode_patch = intel_microcode_find();

	if (!microcode_patch)
		printk(BIOS_ERR, "microcode not found in CBFS!\n");

	intel_microcode_load_unlocked(microcode_patch);

	/* TODO: Handle mp_init_with_smm failure? */
	mp_init_with_smm(dev->link_list, &mp_ops);

	/*
	 * chip_config is used in cpu device callback. Other than cpu 0,
	 * rest of the CPU devices do not have chip_info updated.
	 */
	chip_config = dev->chip_info;

	/* update numa domain for all cpu devices */
	xeonsp_init_cpu_config();
}
