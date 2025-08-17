/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/speedstep.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>
#include "commonlib/bsd/helpers.h"
#include "model_206ax.h"
#include "chip.h"
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/common/common.h>
#include <smbios.h>
#include <smp/node.h>
#include <static.h>
#include <types.h>

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

int cpu_config_tdp_levels(void)
{
	msr_t platform_info;

	/* Minimum CPU revision */
	if (cpuid_eax(1) < IVB_CONFIG_TDP_MIN_CPUID)
		return 0;

	/* Bits 34:33 indicate how many levels supported */
	platform_info = rdmsr(MSR_PLATFORM_INFO);
	return (platform_info.hi >> 1) & 3;
}

/*
 * Configure processor power limits if possible
 * This must be done AFTER set of BIOS_RESET_CPL
 */
void set_power_limits(u8 power_limit_1_time)
{
	struct cpu_intel_model_206ax_config *conf = DEV_PTR(cpu_bus)->chip_info;
	msr_t msr = rdmsr(MSR_PLATFORM_INFO);
	msr_t limit;
	unsigned int power_unit;
	unsigned int tdp, min_power, max_power, max_time;
	u8 power_limit_1_val;

	if (power_limit_1_time >= ARRAY_SIZE(power_limit_time_sec_to_msr))
		return;

	if (!(msr.lo & PLATFORM_INFO_SET_TDP))
		return;

	/* Get units */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 2 << ((msr.lo & 0xf) - 1);

	/* Get power defaults for this SKU */
	msr = rdmsr(MSR_PKG_POWER_SKU);
	tdp = msr.lo & 0x7fff;
	min_power = (msr.lo >> 16) & 0x7fff;
	max_power = msr.hi & 0x7fff;
	max_time = (msr.hi >> 16) & 0x7f;

	printk(BIOS_DEBUG, "CPU TDP: %u Watts\n", tdp / power_unit);

	if (power_limit_time_msr_to_sec[max_time] > power_limit_1_time)
		power_limit_1_time = power_limit_time_msr_to_sec[max_time];

	if (min_power > 0 && tdp < min_power)
		tdp = min_power;

	if (max_power > 0 && tdp > max_power)
		tdp = max_power;

	power_limit_1_val = power_limit_time_sec_to_msr[power_limit_1_time];

	limit.lo = 0;
	if (conf->pl1_mw) {
		printk(BIOS_DEBUG, "Setting PL1 to %u milliwatts\n", conf->pl1_mw);
		limit.lo |= ((conf->pl1_mw * power_unit) / 1000) & PKG_POWER_LIMIT_MASK;
	} else {
		/* Set long term power limit to TDP */
		limit.lo |= tdp & PKG_POWER_LIMIT_MASK;
	}
	if (conf->pl2_clamp) {
		printk(BIOS_DEBUG, "Enabling PL1 clamping limitation\n");
		limit.lo |= PKG_POWER_LIMIT_CLAMP;
	}
	limit.lo |= PKG_POWER_LIMIT_EN;
	limit.lo |= (power_limit_1_val & PKG_POWER_LIMIT_TIME_MASK) <<
		PKG_POWER_LIMIT_TIME_SHIFT;

	limit.hi = 0;
	if (conf->pl2_mw) {
		printk(BIOS_DEBUG, "Setting PL2 to %u milliwatts\n", conf->pl2_mw);
		limit.hi |= ((conf->pl2_mw * power_unit) / 1000) & PKG_POWER_LIMIT_MASK;
	} else {
		/* Set short term power limit to 1.25 * TDP */
		limit.hi |= ((tdp * 125) / 100) & PKG_POWER_LIMIT_MASK;
	}
	if (conf->pl2_clamp) {
		printk(BIOS_DEBUG, "Enabling PL2 clamping limitation\n");
		limit.hi |= PKG_POWER_LIMIT_CLAMP;
	}
	limit.hi |= PKG_POWER_LIMIT_EN;
	/* Power limit 2 time is only programmable on SNB EP/EX */

	wrmsr(MSR_PKG_POWER_LIMIT, limit);

	/* Use nominal TDP values for CPUs with configurable TDP */
	if (cpu_config_tdp_levels()) {
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		limit.hi = 0;
		limit.lo = msr.lo & 0xff;
		wrmsr(MSR_TURBO_ACTIVATION_RATIO, limit);
	}
}

static void configure_turbo_ratio_limits(struct cpu_intel_model_206ax_config *conf)
{
	msr_t msr = rdmsr(MSR_TURBO_RATIO_LIMIT);

	for (int i = 0; i < ARRAY_SIZE(conf->turbo_limits.raw); i++) {
		const int shift = i * 8;
		const int limit = conf->turbo_limits.raw[i];

		if (limit) {
			msr.lo &= ~(0xff << shift);
			msr.lo |= (limit << shift);
		}
	}

	wrmsr(MSR_TURBO_RATIO_LIMIT, msr);
}

static void configure_c_states(struct device *dev)
{
	struct cpu_intel_model_206ax_config *conf = dev->upstream->dev->chip_info;
	msr_t msr;

	msr = rdmsr(MSR_PKG_CST_CONFIG_CONTROL);
	msr.lo |= (1 << 28);	// C1 Auto Undemotion Enable
	msr.lo |= (1 << 27);	// C3 Auto Undemotion Enable
	msr.lo |= (1 << 26);	// C1 Auto Demotion Enable
	msr.lo |= (1 << 25);	// C3 Auto Demotion Enable
	msr.lo &= ~(1 << 10);	// Disable IO MWAIT redirection
	msr.lo |= 7;		// No package C-state limit

	msr.lo |= (1 << 15);	// Lock C-State MSR
	wrmsr(MSR_PKG_CST_CONFIG_CONTROL, msr);

	if (boot_cpu()) {
		/*
		 * The following MSRs are in scope 'Package', thus it's sufficient
		 * to write them once on one core.
		 */

		msr = rdmsr(MSR_MISC_PWR_MGMT);
		msr.lo &= ~(1 << 0);	// Enable P-state HW_ALL coordination
		wrmsr(MSR_MISC_PWR_MGMT, msr);

		msr = rdmsr(MSR_POWER_CTL);
		msr.lo |= (1 << 18);	// Enable Energy Perf Bias MSR 0x1b0
		msr.lo |= (1 << 1);	// C1E Enable
		msr.lo |= (1 << 0);	// Bi-directional PROCHOT#
		wrmsr(MSR_POWER_CTL, msr);

		/* C3 Interrupt Response Time Limit */
		msr.hi = 0;
		if (IS_IVY_CPU(cpu_get_cpuid()))
			msr.lo = IRTL_VALID | IRTL_1024_NS | 0x3b;
		else
			msr.lo = IRTL_VALID | IRTL_1024_NS | 0x50;
		wrmsr(MSR_PKGC3_IRTL, msr);

		/* C6 Interrupt Response Time Limit */
		msr.hi = 0;
		if (IS_IVY_CPU(cpu_get_cpuid()))
			msr.lo = IRTL_VALID | IRTL_1024_NS | 0x50;
		else
			msr.lo = IRTL_VALID | IRTL_1024_NS | 0x68;
		wrmsr(MSR_PKGC6_IRTL, msr);

		/* C7 Interrupt Response Time Limit */
		msr.hi = 0;
		if (IS_IVY_CPU(cpu_get_cpuid()))
			msr.lo = IRTL_VALID | IRTL_1024_NS | 0x57;
		else
			msr.lo = IRTL_VALID | IRTL_1024_NS | 0x6D;
		wrmsr(MSR_PKGC7_IRTL, msr);

		/* Primary Plane Current Limit (Icc) */
		msr = rdmsr(MSR_PP0_CURRENT_CONFIG);
		msr.lo &= ~0x1fff;
		if (conf->pp0_current_limit) {
			/* Fill in board specific maximum current supported by VR */
			msr.lo |= conf->pp0_current_limit * 8;
		} else {
			printk(BIOS_INFO, "%s: PP0 current limit not set in devicetree\n", dev_path(dev));
			/*
			 * The default value might over-stress the voltage regulator or
			 * prevent OC on boards with regulators that can handle currents
			 * above the Intel recommendation.
			 */
			msr.lo |= PP0_CURRENT_LIMIT;
		}
		for (int i = 0; i < VR12_PSI_MAX; i++) {
			/*
			 * Light load optimization. Depending on the VR output filter the
			 * number of phases can be reduced at light load. This is a board
			 * specific setting.
			 */
			if (conf->pp0_psi[i].phases != VR12_KEEP_DEFAULT) {
				msr.hi &= ~(0x3ff << (i * 10));
				msr.hi |= (conf->pp0_psi[i].phases - 1) << (i * 10 + 7);
				msr.hi |= conf->pp0_psi[i].current << (i * 10);
			} else {
				printk(BIOS_INFO, "%s: PP0 PSI%d not set in devicetree\n", dev_path(dev), i);
			}
		}
		msr.lo |= PP0_CURRENT_LIMIT_LOCK;
		wrmsr(MSR_PP0_CURRENT_CONFIG, msr);

		/* Secondary Plane Current Limit (IAXG) */
		msr = rdmsr(MSR_PP1_CURRENT_CONFIG);
		msr.lo &= ~0x1fff;
		if (conf->pp1_current_limit) {
			/* Fill in board specific maximum current supported by VR */
			msr.lo |= conf->pp1_current_limit * 8;
		} else {
			printk(BIOS_INFO, "%s: PP1 current limit not set in devicetree\n", dev_path(dev));
			/*
			 * The default value might over-stress the voltage regulator or
			 * prevent OC on boards with regulators that can handle currents
			 * above the Intel recommendation.
			 */
			if (IS_IVY_CPU(cpu_get_cpuid()))
				msr.lo |= PP1_CURRENT_LIMIT_IVB;
			else
				msr.lo |= PP1_CURRENT_LIMIT_SNB;
		}
		for (int i = 0; i < VR12_PSI_MAX; i++) {
			/*
			 * Light load optimization. Depending on the VR output filter the
			 * number of phases can be reduced at light load. This is a board
			 * specific setting.
			 */
			if (conf->pp1_psi[i].phases != VR12_KEEP_DEFAULT) {
				msr.hi &= ~(0x3ff << (i * 10));
				msr.hi |= (conf->pp1_psi[i].phases - 1) << (i * 10 + 7);
				msr.hi |= conf->pp1_psi[i].current << (i * 10);
			} else {
				printk(BIOS_INFO, "%s: PP1 PSI%d not set in devicetree\n", dev_path(dev), i);
			}
		}
		msr.lo |= PP1_CURRENT_LIMIT_LOCK;
		wrmsr(MSR_PP1_CURRENT_CONFIG, msr);
	}

	msr = rdmsr(MSR_PLATFORM_INFO);
	if (msr.lo & PLATFORM_INFO_SET_TURBO_LIMIT) {
		configure_turbo_ratio_limits(conf);
	} else {
		printk(BIOS_INFO, "%s: Programmable ratio limit for turbo mode is disabled\n",
		       dev_path(dev));
	}
}

static void configure_thermal_target(struct device *dev)
{
	struct cpu_intel_model_206ax_config *conf = dev->upstream->dev->chip_info;
	msr_t msr;

	if (boot_cpu()) {
		/*
		 * The following MSR is in scope 'Package', thus it's sufficient
		 * to write it once on one core.
		 */

		/* Set TCC activation offset if supported */
		msr = rdmsr(MSR_PLATFORM_INFO);
		if ((msr.lo & (1 << 30)) && conf->tcc_offset) {
			msr = rdmsr(MSR_TEMPERATURE_TARGET);
			msr.lo &= ~(0xf << 24); /* Bits 27:24 */
			msr.lo |= (conf->tcc_offset & 0xf) << 24;
			wrmsr(MSR_TEMPERATURE_TARGET, msr);
		}
	}
}

static void configure_misc(void)
{
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 0);	  /* Fast String enable */
	msr.lo |= (1 << 3);	  /* TM1/TM2/EMTTM enable */
	msr.lo |= (1 << 16);	  /* Enhanced SpeedStep Enable */
	wrmsr(IA32_MISC_ENABLE, msr);

	/* Disable Thermal interrupts */
	msr.lo = 0;
	msr.hi = 0;
	wrmsr(IA32_THERM_INTERRUPT, msr);

	if (boot_cpu()) {
		/*
		 * The following MSR is in scope 'Package', thus it's sufficient
		 * to write it once on one core.
		 */

		/* Enable package critical interrupt only */
		msr.lo = 1 << 4;
		msr.hi = 0;
		wrmsr(IA32_PACKAGE_THERM_INTERRUPT, msr);
	}
}

static void set_max_ratio(void)
{
	msr_t msr, perf_ctl;

	perf_ctl.hi = 0;

	/* Check for configurable TDP option */
	if (cpu_config_tdp_levels()) {
		/* Set to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		perf_ctl.lo = (msr.lo & 0xff) << 8;
	} else {
		/* Platform Info bits 15:8 give max ratio */
		msr = rdmsr(MSR_PLATFORM_INFO);
		perf_ctl.lo = msr.lo & 0xff00;
	}
	wrmsr(IA32_PERF_CTL, perf_ctl);

	printk(BIOS_DEBUG, "model_x06ax: frequency set to %d\n",
	       ((perf_ctl.lo >> 8) & 0xff) * SANDYBRIDGE_BCLK);
}

unsigned int smbios_cpu_get_max_speed_mhz(void)
{
	msr_t msr;
	msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
	return (msr.lo & 0xff) * SANDYBRIDGE_BCLK;
}

unsigned int smbios_cpu_get_current_speed_mhz(void)
{
	msr_t msr;
	msr = rdmsr(MSR_PLATFORM_INFO);
	return ((msr.lo >> 8) & 0xff) * SANDYBRIDGE_BCLK;
}

unsigned int smbios_processor_external_clock(void)
{
	return SANDYBRIDGE_BCLK;
}

static void model_206ax_report(void)
{
	static const char *const mode[] = {"NOT ", ""};
	char processor_name[49];
	int vt, txt, aes;
	uint32_t cpu_id, cpu_feature_flag;

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	/* CPUID and features */
	cpu_id = cpu_get_cpuid();
	printk(BIOS_INFO, "CPU: cpuid(1) 0x%x\n", cpu_id);

	cpu_feature_flag = cpu_get_feature_flags_ecx();
	aes = (cpu_feature_flag & CPUID_AES) ? 1 : 0;
	txt = (cpu_feature_flag & CPUID_SMX) ? 1 : 0;
	vt = (cpu_feature_flag & CPUID_VMX) ? 1 : 0;
	printk(BIOS_INFO, "CPU: AES %ssupported\n", mode[aes]);
	printk(BIOS_INFO, "CPU: TXT %ssupported\n", mode[txt]);
	printk(BIOS_INFO, "CPU: VT %ssupported\n", mode[vt]);
}

static void model_206ax_init(struct device *cpu)
{
	/* Clear out pending MCEs */
	/* This should only be done on a cold boot */
	mca_clear_status();

	/* Print infos */
	model_206ax_report();

	/* Setup Page Attribute Tables (PAT) */
	// TODO set up PAT

	enable_lapic_tpr();

	/* Set virtualization based on Kconfig option */
	set_vmx_and_lock();

	/* Configure C States */
	configure_c_states(cpu);

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	/* Thermal throttle activation offset */
	configure_thermal_target(cpu);

	set_aesni_lock();

	/* Enable Direct Cache Access */
	configure_dca_cap();

	/* Set energy policy */
	set_energy_perf_bias(ENERGY_POLICY_NORMAL);

	/* Set Max Ratio */
	set_max_ratio();

	/* Enable Turbo */
	enable_turbo();
}

/* MP initialization support. */
static void pre_mp_init(void)
{
	/* Setup MTRRs based on physical address size. */
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

static int get_cpu_count(void)
{
	msr_t msr;
	unsigned int num_threads;
	unsigned int num_cores;

	msr = rdmsr(MSR_CORE_THREAD_COUNT);
	num_threads = (msr.lo >> 0) & 0xffff;
	num_cores = (msr.lo >> 16) & 0xffff;
	printk(BIOS_DEBUG, "CPU has %u cores, %u threads enabled.\n",
	       num_cores, num_threads);

	return num_threads;
}

static void get_microcode_info(const void **microcode, int *parallel)
{
	*microcode = intel_microcode_find();
	*parallel = !intel_ht_supported();
}

static void per_cpu_smm_trigger(void)
{
	/* Relocate the SMM handler. */
	smm_relocate();

	/* After SMM relocation a 2nd microcode load is required. */
	const void *microcode_patch = intel_microcode_find();
	intel_microcode_load_unlocked(microcode_patch);
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
	.pre_mp_smm_init = smm_initialize,
	.per_cpu_smm_trigger = per_cpu_smm_trigger,
	.relocation_handler = smm_relocation_handler,
	.post_mp_init = post_mp_init,
};

void mp_init_cpus(struct bus *cpu_bus)
{
	/* TODO: Handle mp_init_with_smm failure? */
	mp_init_with_smm(cpu_bus, &mp_ops);

	/* pre_mp_init made the flash not cacheable. Reset to WP for performance. */
	mtrr_use_temp_range(CACHE_ROM_BASE, CAR_CACHE_ROM_SIZE,
			    MTRR_TYPE_WRPROT);
}

static struct device_operations cpu_dev_ops = {
	.init     = model_206ax_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x206a0, CPUID_ALL_STEPPINGS_MASK }, /* Intel Sandybridge */
	{ X86_VENDOR_INTEL, 0x306a0, CPUID_ALL_STEPPINGS_MASK }, /* Intel IvyBridge */
	CPU_TABLE_END
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
