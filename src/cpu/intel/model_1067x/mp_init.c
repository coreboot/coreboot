/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/common/common.h>
#include <device/device.h>

/* Parallel MP initialization support. */
static void pre_mp_init(void)
{
	const void *patch = intel_microcode_find();
	intel_microcode_load_unlocked(patch);

	/* Setup MTRRs based on physical address size. */
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

static int get_cpu_count(void)
{
	const struct cpuid_result cpuid1 = cpuid(1);
	const char cores = (cpuid1.ebx >> 16) & 0xf;

	printk(BIOS_DEBUG, "CPU has %u cores.\n", cores);

	return cores;
}

static void get_microcode_info(const void **microcode, int *parallel)
{
	*microcode = intel_microcode_find();
	*parallel = !intel_ht_supported();
}

/* the SMRR enable and lock bit need to be set in IA32_FEATURE_CONTROL
   to enable SMRR so configure IA32_FEATURE_CONTROL early on */
static void pre_mp_smm_init(void)
{
	smm_initialize();
}

#define SMRR_SUPPORTED (1 << 11)

static void per_cpu_smm_trigger(void)
{
	msr_t mtrr_cap = rdmsr(MTRR_CAP_MSR);
	if (cpu_has_alternative_smrr() && mtrr_cap.lo & SMRR_SUPPORTED) {
		set_feature_ctrl_vmx();
		msr_t ia32_ft_ctrl = rdmsr(IA32_FEATURE_CONTROL);
		/* We don't care if the lock is already setting
		   as our smm relocation handler is able to handle
		   setups where SMRR is not enabled here. */
		if (ia32_ft_ctrl.lo & (1 << 0)) {
			/* IA32_FEATURE_CONTROL locked. If we set it again we
			   get an illegal instruction. */
			printk(BIOS_DEBUG, "IA32_FEATURE_CONTROL already locked\n");
			printk(BIOS_DEBUG, "SMRR status: %senabled\n",
			       ia32_ft_ctrl.lo & (1 << 3) ? "" : "not ");
		} else {
			if (!CONFIG(SET_IA32_FC_LOCK_BIT))
				printk(BIOS_INFO,
				       "Overriding CONFIG(SET_IA32_FC_LOCK_BIT) to enable SMRR\n");
			ia32_ft_ctrl.lo |= (1 << 3) | (1 << 0);
			wrmsr(IA32_FEATURE_CONTROL, ia32_ft_ctrl);
		}
	} else {
		set_vmx_and_lock();
	}

	/* Relocate the SMM handler. */
	smm_relocate();
}

static void post_mp_init(void)
{
	/* Now that all APs have been relocated as well as the BSP let SMIs
	 * start flowing. */
	global_smi_enable();

	/* Lock down the SMRAM space. */
	smm_lock();
}

static const struct mp_ops mp_ops = {
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_cpu_count,
	.get_smm_info = smm_info,
	.get_microcode_info = get_microcode_info,
	.pre_mp_smm_init = pre_mp_smm_init,
	.per_cpu_smm_trigger = per_cpu_smm_trigger,
	.relocation_handler = smm_relocation_handler,
	.post_mp_init = post_mp_init,
};

void mp_init_cpus(struct bus *cpu_bus)
{
	if (mp_init_with_smm(cpu_bus, &mp_ops))
		printk(BIOS_ERR, "MP initialization failure.\n");
}
