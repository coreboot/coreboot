/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/common/common.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <device/pci.h>
#include <fsp/api.h>
#include <intelblocks/acpi.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/msr.h>
#include <intelblocks/pmclib.h>
#include <smbios.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/soc_chip.h>
#include <static.h>

bool cpu_soc_is_in_untrusted_mode(void)
{
	msr_t msr;

	msr = rdmsr(MSR_BIOS_DONE);
	return !!(msr.lo & ENABLE_IA_UNTRUSTED);
}

void cpu_soc_bios_done(void)
{
	msr_t msr;

	msr = rdmsr(MSR_BIOS_DONE);
	msr.lo |= ENABLE_IA_UNTRUSTED;
	wrmsr(MSR_BIOS_DONE, msr);
}

uint8_t get_supported_lpm_mask(void)
{
	const config_t *conf = config_of_soc();
	if (!conf->s0ix_enable)
		return 0;

	return LPM_S0i2_0 | LPM_S0i2_1 | LPM_S0i2_2;
}

static void soc_fsp_load(void)
{
	fsps_load();
}

static void configure_misc(void)
{
	msr_t msr;

	const struct soc_intel_pantherlake_config *conf = config_of_soc();

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= FAST_STRINGS_ENABLE_BIT;
	msr.lo |= TM1_TM2_EMTTM_ENABLE_BIT;
	wrmsr(IA32_MISC_ENABLE, msr);

	/* Set EIST status */
	cpu_set_eist(conf->eist_enable);

	/* Disable Thermal interrupts */
	msr.lo = 0;
	msr.hi = 0;
	wrmsr(IA32_THERM_INTERRUPT, msr);

	/* Enable package critical interrupt only */
	msr.lo = CRITICAL_TEMP_INTERRUPT_ENABLE;
	msr.hi = 0;
	wrmsr(IA32_PACKAGE_THERM_INTERRUPT, msr);

	/* Enable PROCHOT and Power Performance Platform Override */
	msr = rdmsr(MSR_POWER_CTL);
	msr.lo |= ENABLE_BIDIR_PROCHOT;
	msr.lo |= VR_THERM_ALERT_DISABLE_LOCK;
	msr.lo |= PWR_PERF_PLATFORM_OVR;
	wrmsr(MSR_POWER_CTL, msr);
}

enum core_type get_soc_cpu_type(void)
{
	if (cpu_is_hybrid_supported())
		return cpu_get_cpu_type();

	return CPUID_CORE_TYPE_INTEL_CORE;
}

bool soc_is_nominal_freq_supported(void)
{
	return true;
}

static void enable_x2apic(void)
{
	if (!CONFIG(X2APIC_LATE_WORKAROUND))
		return;

	enable_lapic_mode(true);
}

/* All CPUs including BSP will run the following function. */
void soc_core_init(struct device *cpu)
{
	/* Clear out pending MCEs */
	mca_configure();

	enable_x2apic();

	enable_lapic_tpr();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	enable_pm_timer_emulation();

	/* Enable Direct Cache Access */
	configure_dca_cap();

	/* Set energy policy */
	set_energy_perf_bias(ENERGY_POLICY_NORMAL);

	const struct soc_intel_pantherlake_config *conf = config_of_soc();
	/* Set energy-performance preference */
	if (conf != NULL && conf->enable_energy_perf_pref) {
		if (check_energy_perf_cap())
			set_energy_perf_pref(conf->energy_perf_pref_value);
	}

	/* Enable Turbo */
	enable_turbo();

	/* Set core type in struct cpu_info */
	set_dev_core_type();

	if (CONFIG(INTEL_TME) && is_tme_supported())
		set_tme_core_activate();

	if (CONFIG(DROP_CPU_FEATURE_PROGRAM_IN_FSP)) {
		/* Disable 3-strike error */
		disable_signaling_three_strike_event();

		set_aesni_lock();

		/* Enable VMX */
		set_feature_ctrl_vmx_arg(CONFIG(ENABLE_VMX) && !conf->disable_vmx);

		/* Feature control lock configure */
		set_feature_ctrl_lock();
	}
}

static void per_cpu_smm_trigger(void)
{
	/* Relocate the SMM handler. */
	smm_relocate();
}

static void pre_mp_init(void)
{
	soc_fsp_load();

	const struct soc_intel_pantherlake_config *conf = config_of_soc();
	if (conf == NULL) {
		printk(BIOS_ERR, "Configuration could not be retrieved.\n");
		return;
	}
	if (conf->enable_energy_perf_pref) {
		if (check_energy_perf_cap())
			enable_energy_perf_pref();
		else
			printk(BIOS_WARNING, "Energy Performance Preference not supported!\n");
	}
}

static void post_mp_init(void)
{
	/* Set Max Ratio */
	cpu_set_max_ratio();

	/*
	 * 1. Now that all APs have been relocated as well as the BSP let SMIs
	 * start flowing.
	 * 2. Skip enabling power button SMI and enable it after BS_CHIPS_INIT
	 * to avoid shutdown hang due to lack of init on certain IP in FSP-S.
	 */
	global_smi_enable_no_pwrbtn();
}

static const struct mp_ops mp_ops = {
	/*
	 * Skip Pre MP init MTRR programming as MTRRs are mirrored from BSP,
	 * that are set prior to ramstage.
	 * Real MTRRs programming are being done after resource allocation.
	 */
	.pre_mp_init = pre_mp_init,
	.get_cpu_count = get_cpu_count,
	.get_smm_info = smm_info,
	.get_microcode_info = get_microcode_info,
	.pre_mp_smm_init = smm_initialize,
	.per_cpu_smm_trigger = per_cpu_smm_trigger,
	.relocation_handler = smm_relocation_handler,
	.post_mp_init = post_mp_init,
};

void mp_init_cpus(struct bus *cpu_bus)
{
	if (mp_init_with_smm(cpu_bus, &mp_ops))
		printk(BIOS_ERR, "MP initialization failure.\n");

	/* Thermal throttle activation offset */
	configure_tcc_thermal_target();
}

int soc_skip_ucode_update(u32 current_patch_id, u32 new_patch_id)
{
	if (!CONFIG(CHROMEOS))
		return 0;
	/*
	 * Locked RO Descriptor Implications:
	 *
	 * - A locked descriptor signals the RO binary is fixed; the FIT will load the
	 *   RO's microcode during system reset.
	 * - Attempts to load newer microcode from the RW CBFS will cause a boot-time
	 *   delay (~60ms, core-dependent), as the microcode must be reloaded on BSP+APs.
	 * - The kernel can load microcode updates without impacting AP FW boot time.
	 * - Skipping RW CBFS microcode loading is low-risk when the RO is locked,
	 *   prioritizing fast boot times.
	 */
	if (CONFIG(LOCK_MANAGEMENT_ENGINE) && current_patch_id)
		return 1;

	return 0;
}

/* Override SMBIOS type 4 processor serial numbers */
const char *smbios_processor_serial_number(void)
{
	char *qdf = retrieve_soc_qdf_info_via_pmc_ipc();

	if (qdf != NULL)
		return qdf;
	else
		return "";
}
