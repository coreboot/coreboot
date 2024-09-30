/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <device/pci_ops.h>
#include <drivers/intel/dptf/chip.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/power_limit.h>
#include <soc/msr.h>
#include <soc/pci_devs.h>
#include <soc/soc_chip.h>
#include <soc/systemagent.h>
#include <static.h>

/* Convert time in seconds to POWER_LIMIT_1_TIME MSR value */
static const u8 power_limit_time_sec_to_msr[] = {
	[0]   = 0x00,
	[1]   = 0x0a,
	[2]   = 0x0b,
	[3]   = 0x4b,
	[4]   = 0x0c,
	[5]   = 0x2c,
	[6]   = 0x4c,
	[7]   = 0x6c,
	[8]   = 0x0d,
	[10]  = 0x2d,
	[12]  = 0x4d,
	[14]  = 0x6d,
	[16]  = 0x0e,
	[20]  = 0x2e,
	[24]  = 0x4e,
	[28]  = 0x6e,
	[32]  = 0x0f,
	[40]  = 0x2f,
	[48]  = 0x4f,
	[56]  = 0x6f,
	[64]  = 0x10,
	[80]  = 0x30,
	[96]  = 0x50,
	[112] = 0x70,
	[128] = 0x11,
};

/* Convert POWER_LIMIT_1_TIME MSR value to seconds */
static const u8 power_limit_time_msr_to_sec[] = {
	[0x00] = 0,
	[0x0a] = 1,
	[0x0b] = 2,
	[0x4b] = 3,
	[0x0c] = 4,
	[0x2c] = 5,
	[0x4c] = 6,
	[0x6c] = 7,
	[0x0d] = 8,
	[0x2d] = 10,
	[0x4d] = 12,
	[0x6d] = 14,
	[0x0e] = 16,
	[0x2e] = 20,
	[0x4e] = 24,
	[0x6e] = 28,
	[0x0f] = 32,
	[0x2f] = 40,
	[0x4f] = 48,
	[0x6f] = 56,
	[0x10] = 64,
	[0x30] = 80,
	[0x50] = 96,
	[0x70] = 112,
	[0x11] = 128,
};

/*
 * Configure processor power limits if possible
 * This must be done AFTER set of BIOS_RESET_CPL
 */
void set_power_limits(u8 power_limit_1_time,
		struct soc_power_limits_config *conf)
{
	msr_t msr;
	msr_t limit;
	unsigned int power_unit;
	unsigned int tdp, min_power, max_power, max_time, tdp_pl2, tdp_pl1;
	u8 power_limit_1_val;
	uint32_t value;

	if (CONFIG(SOC_INTEL_DISABLE_POWER_LIMITS)) {
		printk(BIOS_INFO, "Disabling RAPL\n");
		if (CONFIG(SOC_INTEL_RAPL_DISABLE_VIA_MCHBAR)) {
			value = MCHBAR32(MCH_PKG_POWER_LIMIT_LO);
			MCHBAR32(MCH_PKG_POWER_LIMIT_LO) = value & ~(PKG_POWER_LIMIT_EN);
			value = MCHBAR32(MCH_PKG_POWER_LIMIT_HI);
			MCHBAR32(MCH_PKG_POWER_LIMIT_HI) = value & ~(PKG_POWER_LIMIT_EN);
		} else {
			msr = rdmsr(MSR_PKG_POWER_LIMIT);
			msr.lo &= ~PKG_POWER_LIMIT_EN;
			msr.hi &= ~PKG_POWER_LIMIT_EN;
			wrmsr(MSR_PKG_POWER_LIMIT, msr);
		}
		return;
	}

	if (power_limit_1_time >= ARRAY_SIZE(power_limit_time_sec_to_msr))
		power_limit_1_time = ARRAY_SIZE(power_limit_time_sec_to_msr) - 1;

	msr = rdmsr(MSR_PLATFORM_INFO);
	if (!(msr.lo & PLATFORM_INFO_SET_TDP))
		return;

	/* Get units */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 1 << (msr.lo & 0xf);

	/* Get power defaults for this SKU */
	msr = rdmsr(MSR_PKG_POWER_SKU);
	tdp = msr.lo & 0x7fff;
	min_power = (msr.lo >> 16) & 0x7fff;
	max_power = msr.hi & 0x7fff;
	max_time = (msr.hi >> 16) & 0x7f;

	printk(BIOS_INFO, "CPU TDP = %u Watts\n", tdp / power_unit);

	if (power_limit_time_msr_to_sec[max_time] > power_limit_1_time)
		power_limit_1_time = power_limit_time_msr_to_sec[max_time];

	if (min_power > 0 && tdp < min_power)
		tdp = min_power;

	if (max_power > 0 && tdp > max_power)
		tdp = max_power;

	power_limit_1_val = power_limit_time_sec_to_msr[power_limit_1_time];

	/* Set long term power limit to TDP */
	limit.lo = 0;
	tdp_pl1 = ((conf->tdp_pl1_override == 0) ?
			tdp : (conf->tdp_pl1_override * power_unit));
	printk(BIOS_INFO, "CPU PL1 = %u Watts\n", tdp_pl1 / power_unit);
	limit.lo |= (tdp_pl1 & PKG_POWER_LIMIT_MASK);

	/* Set PL1 Pkg Power clamp bit */
	limit.lo |= PKG_POWER_LIMIT_CLAMP;

	limit.lo |= PKG_POWER_LIMIT_EN;
	limit.lo |= (power_limit_1_val & PKG_POWER_LIMIT_TIME_MASK) <<
		PKG_POWER_LIMIT_TIME_SHIFT;

	/* Set short term power limit to 1.25 * TDP if no config given */
	limit.hi = 0;
	tdp_pl2 = (conf->tdp_pl2_override == 0) ?
		(tdp * 125) / 100 : (conf->tdp_pl2_override * power_unit);
	printk(BIOS_INFO, "CPU PL2 = %u Watts\n", tdp_pl2 / power_unit);
	limit.hi |= (tdp_pl2) & PKG_POWER_LIMIT_MASK;
	limit.hi |= PKG_POWER_LIMIT_CLAMP;
	limit.hi |= PKG_POWER_LIMIT_EN;

	/* Power limit 2 time is only programmable on server SKU */
	wrmsr(MSR_PKG_POWER_LIMIT, limit);

	/* Set PL2 power limit values in MCHBAR and disable PL1 */
	MCHBAR32(MCH_PKG_POWER_LIMIT_LO) = limit.lo & (~(PKG_POWER_LIMIT_EN));
	MCHBAR32(MCH_PKG_POWER_LIMIT_HI) = limit.hi;

	/* Set PsysPl2 */
	if (conf->tdp_psyspl2) {
		limit = rdmsr(MSR_PLATFORM_POWER_LIMIT);
		limit.hi = 0;
		printk(BIOS_INFO, "CPU PsysPL2 = %u Watts\n",
					conf->tdp_psyspl2);
		limit.hi |= (conf->tdp_psyspl2 * power_unit) &
				PKG_POWER_LIMIT_MASK;
		limit.hi |= PKG_POWER_LIMIT_CLAMP;
		limit.hi |= PKG_POWER_LIMIT_EN;
		wrmsr(MSR_PLATFORM_POWER_LIMIT, limit);
	}

	/* Set PsysPl3 */
	if (conf->tdp_psyspl3) {
		limit = rdmsr(MSR_PL3_CONTROL);
		limit.lo = 0;
		printk(BIOS_INFO, "CPU PsysPL3 = %u Watts\n",
					conf->tdp_psyspl3);
		limit.lo |= (conf->tdp_psyspl3 * power_unit) &
				PKG_POWER_LIMIT_MASK;
		/* Enable PsysPl3 */
		limit.lo |= PKG_POWER_LIMIT_EN;
		/* set PsysPl3 time window */
		limit.lo |= (conf->tdp_psyspl3_time &
				PKG_POWER_LIMIT_TIME_MASK) <<
				PKG_POWER_LIMIT_TIME_SHIFT;
		/* set PsysPl3 duty cycle */
		limit.lo |= (conf->tdp_psyspl3_dutycycle &
				PKG_POWER_LIMIT_DUTYCYCLE_MASK) <<
				PKG_POWER_LIMIT_DUTYCYCLE_SHIFT;
		wrmsr(MSR_PL3_CONTROL, limit);
	}

	/* Set Pl4 */
	if (conf->tdp_pl4) {
		limit = rdmsr(MSR_VR_CURRENT_CONFIG);
		limit.lo = 0;
		printk(BIOS_INFO, "CPU PL4 = %u Watts\n", conf->tdp_pl4);
		limit.lo |= (conf->tdp_pl4 * power_unit) &
				PKG_POWER_LIMIT_MASK;
		wrmsr(MSR_VR_CURRENT_CONFIG, limit);
	}

	/* Set DDR RAPL power limit by copying from MMIO to MSR */
	msr.lo = MCHBAR32(MCH_DDR_POWER_LIMIT_LO);
	msr.hi = MCHBAR32(MCH_DDR_POWER_LIMIT_HI);
	wrmsr(MSR_DDR_RAPL_LIMIT, msr);

	/* Use nominal TDP values for CPUs with configurable TDP */
	if (cpu_config_tdp_levels()) {
		limit.hi = 0;
		limit.lo = cpu_get_tdp_nominal_ratio();
		wrmsr(MSR_TURBO_ACTIVATION_RATIO, limit);
	}
}

u8 get_cpu_tdp(void)
{
	unsigned int power_unit, cpu_tdp;

	/* Get units */
	msr_t msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 1 << (msr.lo & 0xf);

	/* Get power defaults for this SKU */
	msr = rdmsr(MSR_PKG_POWER_SKU);
	cpu_tdp = msr.lo & 0x7fff;

	return cpu_tdp / power_unit;
}

WEAK_DEV_PTR(dptf_policy);

#if CONFIG(SOC_INTEL_COMMON_BLOCK_VARIANT_POWER_LIMIT)
void variant_update_cpu_power_limits(const struct cpu_tdp_power_limits *limits,
		size_t num_entries)
{
	if (!num_entries) {
		printk(BIOS_INFO, "CPU Power limits entry not available\n");
		return;
	}

	const struct device *policy_dev = DEV_PTR(dptf_policy);
	if (!policy_dev)
		return;

	struct drivers_intel_dptf_config *config = policy_dev->chip_info;
	if (!config) {
		printk(BIOS_INFO, "DPTF is not enabled\n");
		return;
	}

	uint16_t mch_id = pci_s_read_config16(PCI_DEV(0, 0, 0), PCI_DEVICE_ID);
	if (mch_id == 0xffff) {
		printk(BIOS_INFO, "No matching PCI DID present\n");
		return;
	}

	uint8_t tdp = get_cpu_tdp();

	for (size_t index = 0; index < num_entries; index++) {
		if (mch_id != limits[index].mch_id || tdp != limits[index].cpu_tdp) {
			continue;
		} else {
			struct dptf_power_limits *settings = &config->controls.power_limits;
			config_t *conf = config_of_soc();
			struct soc_power_limits_config *soc_config = conf->power_limits_config;
			settings->pl1.min_power = limits[index].pl1_min_power;
			settings->pl1.max_power = limits[index].pl1_max_power;
			settings->pl2.min_power = limits[index].pl2_min_power;
			settings->pl2.max_power = limits[index].pl2_max_power;
			soc_config->tdp_pl4 = DIV_ROUND_UP(limits[index].pl4_power, MILLIWATTS_TO_WATTS);
			printk(BIOS_INFO, "Overriding power limits PL1 (mW) (%u, %u) PL2 (mW) (%u, %u) PL4 (W) (%u)\n",
				settings->pl1.min_power,
				settings->pl1.max_power,
				settings->pl2.min_power,
				settings->pl2.max_power,
				soc_config->tdp_pl4);
		}
	}
}
#endif
