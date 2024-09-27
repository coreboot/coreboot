/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/intel/common/common.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/msr.h>
#include <soc/msr.h>
#include <soc/soc_chip.h>
#include <static.h>

static void configure_misc(void)
{
	msr_t msr;
	config_t *conf = config_of_soc();

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= FAST_STRINGS_ENABLE_BIT;
	msr.lo |= (1 << 3); /**< Enable TM1, TM2 and EMTTM. */
	wrmsr(IA32_MISC_ENABLE, msr);

	cpu_set_eist(conf->eist_enable);

	msr.lo = 0;
	msr.hi = 0;
	wrmsr(IA32_THERM_INTERRUPT, msr);

	msr.lo = 1 << 4;
	msr.hi = 0;
	wrmsr(IA32_PACKAGE_THERM_INTERRUPT, msr);

	msr = rdmsr(MSR_POWER_CTL);
	msr.lo |= (1 << 0);  /**< Enable bi-directional PROCHOT as an input. */
	msr.lo |= (1 << 23); /**< Lock it. */
	wrmsr(MSR_POWER_CTL, msr);
}

void soc_core_init(struct device *dev)
{
	/**
	 * This should only be done on a cold boot. Also, some these banks are core
	 * vs package scope.
	 */
	mca_configure();

	enable_lapic_tpr();

	configure_misc();

	configure_dca_cap();

	set_energy_perf_bias(ENERGY_POLICY_NORMAL);

	enable_turbo();
}

static void post_mp_init(void)
{
	cpu_set_max_ratio();

	/**
	 * Now that all APs have been relocated as well as the BSP, let SMI start flowing.
	 */
	global_smi_enable();
}

static const struct mp_ops mp_ops = {
	/**
	 * Skip Pre MP init MTRR programming as MTRRs are mirrored from BSP that are set prior
	 * to ramstage. Real MTRRs programming are being done after resource allocation.
	 */

	.get_cpu_count = get_cpu_count,
#if CONFIG(HAVE_SMI_HANDLER)
	.get_smm_info = smm_info,
#endif
	.get_microcode_info = get_microcode_info,
	.pre_mp_smm_init = smm_initialize,
	.per_cpu_smm_trigger = smm_relocate,
	.relocation_handler = smm_relocation_handler,
	.post_mp_init = post_mp_init,
};

void mp_init_cpus(struct bus *cpu_bus)
{
	if (mp_init_with_smm(cpu_bus, &mp_ops))
		die_with_post_code(POSTCODE_HW_INIT_FAILURE, "mp_init_with_smm failed!\n");

	configure_tcc_thermal_target();
}

bool cpu_soc_is_in_untrusted_mode(void)
{
	msr_t msr;

	msr = rdmsr(MSR_BIOS_DONE);
	return !!(msr.lo & ENABLE_IA_UNTRUSTED);
}
