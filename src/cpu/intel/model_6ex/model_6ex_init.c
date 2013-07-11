/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <cpu/cpu.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/microcode.h>
#include <cpu/intel/hyperthreading.h>
#include <cpu/intel/speedstep.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>

static const uint32_t microcode_updates[] = {
	#include "microcode-1624-m206e839.h"
	#include "microcode-1729-m206ec54.h"
	#include "microcode-1869-m806ec59.h"
	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

#define IA32_FEATURE_CONTROL 0x003a

#define CPUID_VMX (1 << 5)
#define CPUID_SMX (1 << 6)
static void enable_vmx(void)
{
	struct cpuid_result regs;
	msr_t msr;

	msr = rdmsr(IA32_FEATURE_CONTROL);

	if (msr.lo & (1 << 0)) {
		/* VMX locked. If we set it again we get an illegal
		 * instruction
		 */
		return;
	}

	regs = cpuid(1);
	if (regs.ecx & CPUID_VMX) {
		msr.lo |= (1 << 2);
		if (regs.ecx & CPUID_SMX)
			msr.lo |= (1 << 1);
	}

	wrmsr(IA32_FEATURE_CONTROL, msr);

	msr.lo |= (1 << 0); /* Set lock bit */

	wrmsr(IA32_FEATURE_CONTROL, msr);
}

#define HIGHEST_CLEVEL		3
static void configure_c_states(void)
{
	msr_t msr;

	msr = rdmsr(MSR_PMG_CST_CONFIG_CONTROL);
	msr.lo |= (1 << 15); // config lock until next reset.
	msr.lo |= (1 << 10); // Enable I/O MWAIT redirection for C-States
	msr.lo &= ~(1 << 9); // Issue a single stop grant cycle upon stpclk
	// TODO Do we want Deep C4 and  Dynamic L2 shrinking?

	/* Number of supported C-States */
	msr.lo &= ~7;
	msr.lo |= HIGHEST_CLEVEL; // support at most C3

	wrmsr(MSR_PMG_CST_CONFIG_CONTROL, msr);

	/* Set Processor MWAIT IO BASE (P_BLK) */
	msr.hi = 0;
	msr.lo = ((PMB0_BASE + 4) & 0xffff) | (((PMB1_BASE + 9) & 0xffff) << 16);
	wrmsr(MSR_PMG_IO_BASE_ADDR, msr);

	/* set C_LVL controls */
	msr.hi = 0;
	msr.lo = (PMB0_BASE + 4) | ((HIGHEST_CLEVEL - 2) << 16); // -2 because LVL0+1 aren't counted
	wrmsr(MSR_PMG_IO_CAPTURE_ADDR, msr);
}

#define IA32_MISC_ENABLE	0x1a0
static void configure_misc(void)
{
	msr_t msr;

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 3); 	/* TM1 enable */
	msr.lo |= (1 << 13);	/* TM2 enable */
	msr.lo |= (1 << 17);	/* Bidirectional PROCHOT# */

	msr.lo |= (1 << 10);	/* FERR# multiplexing */

	// TODO: Only if  IA32_PLATFORM_ID[17] = 0 and IA32_PLATFORM_ID[50] = 1
	msr.lo |= (1 << 16);	/* Enhanced SpeedStep Enable */

	// TODO Do we want Deep C4 and  Dynamic L2 shrinking?
	wrmsr(IA32_MISC_ENABLE, msr);

	msr.lo |= (1 << 20);	/* Lock Enhanced SpeedStep Enable */
	wrmsr(IA32_MISC_ENABLE, msr);

	// set maximum CPU speed
	msr = rdmsr(IA32_PERF_STS);
	int busratio_max=(msr.hi >> (40-32)) & 0x1f;

	msr = rdmsr(IA32_PLATFORM_ID);
	int vid_max=msr.lo & 0x3f;

	msr.lo &= ~0xffff;
	msr.lo |= busratio_max << 8;
	msr.lo |= vid_max;

	wrmsr(IA32_PERF_CTL, msr);
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

static void model_6ex_init(device_t cpu)
{
	char processor_name[49];

	/* Turn on caching if we haven't already */
	x86_enable_cache();

	/* Update the microcode */
	intel_update_microcode(microcode_updates);

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	/* Setup MTRRs */
	x86_setup_mtrrs();
	x86_mtrr_check();

	/* Enable the local cpu apics */
	setup_lapic();

	/* Enable virtualization */
	enable_vmx();

	/* Configure C States */
	configure_c_states();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc();

	/* PIC thermal sensor control */
	configure_pic_thermal_sensors();

	/* Start up my cpu siblings */
	intel_sibling_init(cpu);
}

static struct device_operations cpu_dev_ops = {
	.init     = model_6ex_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x06e0 }, /* Intel Core Solo/Core Duo */
	{ X86_VENDOR_INTEL, 0x06e8 }, /* Intel Core Solo/Core Duo */
	{ X86_VENDOR_INTEL, 0x06ec }, /* Intel Core Solo/Core Duo */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

