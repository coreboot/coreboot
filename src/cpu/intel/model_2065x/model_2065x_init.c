/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/device.h>
#include <acpi/acpi.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/mp.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/speedstep.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>
#include "model_2065x.h"
#include "chip.h"
#include <cpu/intel/smm_reloc.h>
#include <cpu/intel/common/common.h>
#include <smp/node.h>

/*
 * List of supported C-states in this processor
 *
 * Latencies are typical worst-case package exit time in uS
 * taken from the SandyBridge BIOS specification.
 */
static acpi_cstate_t cstate_map[] = {
	{	/* 0: C0 */
	}, {	/* 1: C1 */
		.latency = 1,
		.power = 1000,
		.resource = {
			.addrl = 0x00,	/* MWAIT State 0 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{	/* 2: C1E */
		.latency = 1,
		.power = 1000,
		.resource = {
			.addrl = 0x01,	/* MWAIT State 0 Sub-state 1 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{	/* 3: C3 */
		.latency = 63,
		.power = 500,
		.resource = {
			.addrl = 0x10,	/* MWAIT State 1 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{	/* 4: C6 */
		.latency = 87,
		.power = 350,
		.resource = {
			.addrl = 0x20,	/* MWAIT State 2 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{	/* 5: C7 */
		.latency = 90,
		.power = 200,
		.resource = {
			.addrl = 0x30,	/* MWAIT State 3 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{	/* 6: C7S */
		.latency = 90,
		.power = 200,
		.resource = {
			.addrl = 0x31,	/* MWAIT State 3 Sub-state 1 */
			.space_id = ACPI_ADDRESS_SPACE_FIXED,
			.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,
			.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,
			.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD,
		}
	},
	{ 0 }
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

static void configure_thermal_target(void)
{
	struct cpu_intel_model_2065x_config *conf;
	struct device *lapic;
	msr_t msr;

	/* Find pointer to CPU configuration */
	lapic = dev_find_lapic(SPEEDSTEP_APIC_MAGIC);
	if (!lapic || !lapic->chip_info)
		return;
	conf = lapic->chip_info;

	/* Set TCC activation offset if supported */
	msr = rdmsr(MSR_PLATFORM_INFO);
	if ((msr.lo & (1 << 30)) && conf->tcc_offset) {
		msr = rdmsr(MSR_TEMPERATURE_TARGET);
		msr.lo &= ~(0xf << 24); /* Bits 27:24 */
		msr.lo |= (conf->tcc_offset & 0xf) << 24;
		wrmsr(MSR_TEMPERATURE_TARGET, msr);
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
}

static void enable_lapic_tpr(void)
{
	msr_t msr;

	msr = rdmsr(MSR_PIC_MSG_CONTROL);
	msr.lo &= ~(1 << 10);	/* Enable APIC TPR updates */
	wrmsr(MSR_PIC_MSG_CONTROL, msr);
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
	       ((perf_ctl.lo >> 8) & 0xff) * IRONLAKE_BCLK);
}

static void configure_mca(void)
{
	msr_t msr;
	int i;

	msr.lo = msr.hi = 0;
	/* This should only be done on a cold boot */
	for (i = 0; i < 7; i++)
		wrmsr(IA32_MC0_STATUS + (i * 4), msr);
}

static void model_2065x_init(struct device *cpu)
{
	char processor_name[49];

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	/* Clear out pending MCEs */
	configure_mca();

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);
	printk(BIOS_INFO, "CPU:lapic=%d, boot_cpu=%d\n", lapicid(),
		boot_cpu());

	/* Setup Page Attribute Tables (PAT) */
	// TODO set up PAT

	/* Enable the local CPU APICs */
	enable_lapic_tpr();
	setup_lapic();

	/* Set virtualization based on Kconfig option */
	set_vmx_and_lock();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	/* Thermal throttle activation offset */
	configure_thermal_target();

	/* Set Max Ratio */
	set_max_ratio();

	/* Enable Turbo */
	enable_turbo();
}

/* MP initialization support. */
static const void *microcode_patch;

static void pre_mp_init(void)
{
	/* Setup MTRRs based on physical address size. */
	x86_setup_mtrrs_with_detect();
	x86_mtrr_check();
}

static int get_cpu_count(void)
{
	msr_t msr;
	int num_threads;
	int num_cores;

	msr = rdmsr(MSR_CORE_THREAD_COUNT);
	num_threads = (msr.lo >> 0) & 0xffff;
	num_cores = (msr.lo >> 16) & 0xffff;
	printk(BIOS_DEBUG, "CPU has %u cores, %u threads enabled.\n",
	       num_cores, num_threads);

	return num_threads;
}

static void get_microcode_info(const void **microcode, int *parallel)
{
	microcode_patch = intel_microcode_find();
	*microcode = microcode_patch;
	*parallel = 1;
}

static void per_cpu_smm_trigger(void)
{
	/* Relocate the SMM handler. */
	smm_relocate();

	/* After SMM relocation a 2nd microcode load is required. */
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
	if (mp_init_with_smm(cpu_bus, &mp_ops))
		printk(BIOS_ERR, "MP initialization failure.\n");
}

static struct device_operations cpu_dev_ops = {
	.init     = model_2065x_init,
};

/* Arrandale / Clarkdale CPU IDs */
static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x20650 },
	{ X86_VENDOR_INTEL, 0x20651 },
	{ X86_VENDOR_INTEL, 0x20652 },
	{ X86_VENDOR_INTEL, 0x20654 },
	{ X86_VENDOR_INTEL, 0x20655 },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
	.cstates  = cstate_map,
};
