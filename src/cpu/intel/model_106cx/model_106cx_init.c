/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/speedstep.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>

#define HIGHEST_CLEVEL		3
static void configure_c_states(void)
{
	msr_t msr;

	msr = rdmsr(MSR_PKG_CST_CONFIG_CONTROL);
	msr.lo |= (1 << 15); // Lock configuration
	msr.lo |= (1 << 10); // redirect IO-based CState transition requests to
			     // MWAIT
	msr.lo &= ~(1 << 9); // Issue a single stop grant cycle upon stpclk
	msr.lo &= ~7; msr.lo |= HIGHEST_CLEVEL; // support at most C3
	// TODO Do we want Deep C4 and  Dynamic L2 shrinking?
	wrmsr(MSR_PKG_CST_CONFIG_CONTROL, msr);

	/* Set Processor MWAIT IO BASE (P_BLK) */
	msr.hi = 0;
	// TODO Do we want PM1_BASE? Needs SMM?
	//msr.lo = ((PMB0_BASE + 4) & 0xffff) | (((PMB1_BASE + 9) & 0xffff)
	//	 << 16);
	msr.lo = ((PMB0_BASE + 4) & 0xffff);
	wrmsr(MSR_PMG_IO_BASE_ADDR, msr);

	/* set C_LVL controls */
	msr.hi = 0;
	// -2 because LVL0+1 aren't counted
	msr.lo = (PMB0_BASE + 4) | ((HIGHEST_CLEVEL - 2) << 16);
	wrmsr(MSR_PMG_IO_CAPTURE_ADDR, msr);
}

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

	// TODO Do we want Deep C4 and  Dynamic L2 shrinking?
	wrmsr(IA32_MISC_ENABLE, msr);

	msr.lo |= (1 << 20);	/* Lock Enhanced SpeedStep Enable */
	wrmsr(IA32_MISC_ENABLE, msr);
}

static void model_106cx_init(struct device *cpu)
{
	char processor_name[49];

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	/* Enable the local CPU APICs */
	setup_lapic();

	/* Configure C States */
	configure_c_states();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	/* TODO: PIC thermal sensor control */
}

static struct device_operations cpu_dev_ops = {
	.init     = model_106cx_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x106c0 }, /* Intel Atom 230 */
	{ X86_VENDOR_INTEL, 0x106ca }, /* Intel Atom D5xx */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};
