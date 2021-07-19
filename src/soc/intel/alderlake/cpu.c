/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on Intel Alder Lake Processor CPU Datasheet
 * Document number: 619501
 * Chapter number: 14
 */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/turbo.h>
#include <cpu/intel/common/common.h>
#include <fsp/api.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/mp_init.h>
#include <intelblocks/msr.h>
#include <soc/cpu.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/soc_chip.h>

static void soc_fsp_load(void)
{
	fsps_load();
}

static void configure_misc(void)
{
	msr_t msr;

	config_t *conf = config_of_soc();

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 0);	/* Fast String enable */
	msr.lo |= (1 << 3);	/* TM1/TM2/EMTTM enable */
	wrmsr(IA32_MISC_ENABLE, msr);

	/* Set EIST status */
	cpu_set_eist(conf->eist_enable);

	/* Disable Thermal interrupts */
	msr.lo = 0;
	msr.hi = 0;
	wrmsr(IA32_THERM_INTERRUPT, msr);

	/* Enable package critical interrupt only */
	msr.lo = 1 << 4;
	msr.hi = 0;
	wrmsr(IA32_PACKAGE_THERM_INTERRUPT, msr);

	/* Enable PROCHOT */
	msr = rdmsr(MSR_POWER_CTL);
	msr.lo |= (1 << 0);	/* Enable Bi-directional PROCHOT as an input*/
	msr.lo |= (1 << 23);	/* Lock it */
	wrmsr(MSR_POWER_CTL, msr);
}

/* All CPUs including BSP will run the following function. */
void soc_core_init(struct device *cpu)
{
	/* Clear out pending MCEs */
	/* TODO(adurbin): This should only be done on a cold boot. Also, some
	 * of these banks are core vs package scope. For now every CPU clears
	 * every bank. */
	mca_configure();

	/* Enable the local CPU apics */
	enable_lapic_tpr();
	setup_lapic();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	enable_pm_timer_emulation();

	/* Enable Direct Cache Access */
	configure_dca_cap();

	/* Set energy policy */
	set_energy_perf_bias(ENERGY_POLICY_NORMAL);

	/* Enable Turbo */
	enable_turbo();
}

static void per_cpu_smm_trigger(void)
{
	/* Relocate the SMM handler. */
	smm_relocate();
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
	.pre_mp_init = soc_fsp_load,
	.get_cpu_count = get_cpu_count,
	.get_smm_info = smm_info,
	.get_microcode_info = get_microcode_info,
	.pre_mp_smm_init = smm_initialize,
	.per_cpu_smm_trigger = per_cpu_smm_trigger,
	.relocation_handler = smm_relocation_handler,
	.post_mp_init = post_mp_init,
};

void soc_init_cpus(struct bus *cpu_bus)
{
	if (mp_init_with_smm(cpu_bus, &mp_ops))
		printk(BIOS_ERR, "MP initialization failure.\n");

	/* Thermal throttle activation offset */
	configure_tcc_thermal_target();
}

enum adl_cpu_type get_adl_cpu_type(void)
{
	const uint16_t adl_m_mch_ids[] = {
		PCI_DEVICE_ID_INTEL_ADL_M_ID_1,
		PCI_DEVICE_ID_INTEL_ADL_M_ID_2,
	};
	const uint16_t adl_p_mch_ids[] = {
		PCI_DEVICE_ID_INTEL_ADL_P_ID_1,
		PCI_DEVICE_ID_INTEL_ADL_P_ID_3,
		PCI_DEVICE_ID_INTEL_ADL_P_ID_4,
		PCI_DEVICE_ID_INTEL_ADL_P_ID_5,
		PCI_DEVICE_ID_INTEL_ADL_P_ID_6,
		PCI_DEVICE_ID_INTEL_ADL_P_ID_7,
		PCI_DEVICE_ID_INTEL_ADL_P_ID_8,
		PCI_DEVICE_ID_INTEL_ADL_P_ID_9,
	};
	const uint16_t adl_s_mch_ids[] = {
		PCI_DEVICE_ID_INTEL_ADL_S_ID_1,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_2,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_3,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_4,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_5,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_6,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_7,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_8,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_9,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_10,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_11,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_12,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_13,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_14,
		PCI_DEVICE_ID_INTEL_ADL_S_ID_15,
	};

	const uint16_t mchid = pci_s_read_config16(PCI_DEV(0, PCI_SLOT(SA_DEVFN_ROOT),
							   PCI_FUNC(SA_DEVFN_ROOT)),
						   PCI_DEVICE_ID);

	for (size_t i = 0; i < ARRAY_SIZE(adl_p_mch_ids); i++) {
		if (adl_p_mch_ids[i] == mchid)
			return ADL_P;
	}

	for (size_t i = 0; i < ARRAY_SIZE(adl_m_mch_ids); i++) {
		if (adl_m_mch_ids[i] == mchid)
			return ADL_M;
	}

	for (size_t i = 0; i < ARRAY_SIZE(adl_s_mch_ids); i++) {
		if (adl_s_mch_ids[i] == mchid)
			return ADL_S;
	}

	return ADL_UNKNOWN;
}

uint8_t get_supported_lpm_mask(void)
{
	enum adl_cpu_type type = get_adl_cpu_type();
	switch (type) {
	case ADL_M: /* fallthrough */
	case ADL_P:
		return LPM_S0i2_0 | LPM_S0i3_0;
	case ADL_S:
		return LPM_S0i2_0 | LPM_S0i2_1;
	default:
		printk(BIOS_ERR, "Unknown ADL CPU type: %d\n", type);
		return 0;
	}
}
