/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>

#define HIGHEST_CLEVEL		3
static void configure_c_states(void)
{
	msr_t msr;

	msr = rdmsr(MSR_PKG_CST_CONFIG_CONTROL);
	msr.lo |= (1 << 15); // config lock until next reset
	msr.lo |= (1 << 14); // Deeper Sleep
	msr.lo |= (1 << 10); // Enable I/O MWAIT redirection for C-States
	msr.lo &= ~(1 << 9); // Issue a single stop grant cycle upon stpclk
	msr.lo |= (1 << 3); // dynamic L2

	/* Number of supported C-States */
	msr.lo &= ~7;
	msr.lo |= HIGHEST_CLEVEL; // support at most C3

	wrmsr(MSR_PKG_CST_CONFIG_CONTROL, msr);

	/* Set Processor MWAIT IO BASE (P_BLK) */
	msr.hi = 0;
	msr.lo = ((PMB0_BASE + 4) & 0xffff) | (((PMB1_BASE + 9) & 0xffff)
		<< 16);
	wrmsr(MSR_PMG_IO_BASE_ADDR, msr);

	/* Set C_LVL controls and IO Capture Address */
	msr.hi = 0;
	// -2 because LVL0+1 aren't counted
	msr.lo = (PMB0_BASE + 4) | ((HIGHEST_CLEVEL - 2) << 16);
	wrmsr(MSR_PMG_IO_CAPTURE_ADDR, msr);
}

#define IA32_PECI_CTL		0x5a0

static void configure_misc(void)
{
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 3);	/* TM1 enable */
	msr.lo |= (1 << 13);	/* TM2 enable */
	msr.lo |= (1 << 17);	/* Bidirectional PROCHOT# */

	msr.lo |= (1 << 10);	/* FERR# multiplexing */

	// TODO: Only if  IA32_PLATFORM_ID[17] = 0 and IA32_PLATFORM_ID[50] = 1
	msr.lo |= (1 << 16);	/* Enhanced SpeedStep Enable */

	/* Enable C2E */
	msr.lo |= (1 << 26);

	/* Enable C4E */
	/* TODO This should only be done on mobile CPUs, see cpuid 5 */
	msr.hi |= (1 << (32 - 32)); // C4E
	msr.hi |= (1 << (33 - 32)); // Hard C4E

	/* Enable EMTTM. */
	/* NOTE: We leave the EMTTM_CR_TABLE0-5 at their default values */
	msr.hi |= (1 << (36 - 32));

	wrmsr(IA32_MISC_ENABLE, msr);

	msr.lo |= (1 << 20);	/* Lock Enhanced SpeedStep Enable */
	wrmsr(IA32_MISC_ENABLE, msr);

	// set maximum CPU speed
	msr = rdmsr(IA32_PERF_STATUS);
	int busratio_max = (msr.hi >> (40-32)) & 0x1f;

	msr = rdmsr(IA32_PLATFORM_ID);
	int vid_max = msr.lo & 0x3f;

	msr.lo &= ~0xffff;
	msr.lo |= busratio_max << 8;
	msr.lo |= vid_max;

	wrmsr(IA32_PERF_CTL, msr);

	/* Enable PECI */
	msr = rdmsr(IA32_PECI_CTL);
	msr.lo |= 1;
	wrmsr(IA32_PECI_CTL, msr);
}

#define PIC_SENS_CFG	0x1aa
static void configure_pic_thermal_sensors(void)
{
	msr_t msr;

	msr = rdmsr(PIC_SENS_CFG);

	msr.lo |= (1 << 21); // inter-core lock TM1
	msr.lo |= (1 << 4); // Enable bypass filter

	wrmsr(PIC_SENS_CFG, msr);
}

static void model_6fx_init(struct device *cpu)
{
	char processor_name[49];

	/* Turn on caching if we haven't already */
	enable_cache();

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	/* Setup Page Attribute Tables (PAT) */
	// TODO set up PAT

	/* Configure C States */
	configure_c_states();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	/* PIC thermal sensor control */
	configure_pic_thermal_sensors();
}

static struct device_operations cpu_dev_ops = {
	.init     = model_6fx_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x06f0 }, /* Intel Core 2 Solo/Core Duo */
	{ X86_VENDOR_INTEL, 0x06f2 }, /* Intel Core 2 Solo/Core Duo */
	{ X86_VENDOR_INTEL, 0x06f6 }, /* Intel Core 2 Solo/Core Duo */
	{ X86_VENDOR_INTEL, 0x06f7 }, /* Intel Core 2 Solo/Core Duo */
	{ X86_VENDOR_INTEL, 0x06fa }, /* Intel Core 2 Solo/Core Duo */
	{ X86_VENDOR_INTEL, 0x06fb }, /* Intel Core 2 Solo/Core Duo */
	{ X86_VENDOR_INTEL, 0x06fd }, /* Intel Core 2 Solo/Core Duo */
	{ X86_VENDOR_INTEL, 0x10661 }, /* Intel Core 2 Celeron Conroe-L */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
