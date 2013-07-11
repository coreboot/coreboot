/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
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
#include <cpu/intel/speedstep.h>
#include <cpu/intel/hyperthreading.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>

static const uint32_t microcode_updates[] = {
	#include "microcode-M01106C2217.h"
	#include "microcode-M01106CA107.h"
	#include "microcode-M04106C2218.h"
	#include "microcode-M04106CA107.h"
	#include "microcode-M08106C2219.h"
	#include "microcode-M08106CA107.h"
	#include "microcode-M10106CA107.h"

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
	msr.lo |= (1 << 15); // Lock configuration
	msr.lo |= (1 << 10); // redirect IO-based CState transition requests to MWAIT
	msr.lo &= ~(1 << 9); // Issue a single stop grant cycle upon stpclk
	msr.lo &= ~7; msr.lo |= HIGHEST_CLEVEL; // support at most C3
	// TODO Do we want Deep C4 and  Dynamic L2 shrinking?
	wrmsr(MSR_PMG_CST_CONFIG_CONTROL, msr);

	/* Set Processor MWAIT IO BASE (P_BLK) */
	msr.hi = 0;
	// TODO Do we want PM1_BASE? Needs SMM?
	//msr.lo = ((PMB0_BASE + 4) & 0xffff) | (((PMB1_BASE + 9) & 0xffff) << 16);
	msr.lo = ((PMB0_BASE + 4) & 0xffff);
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
}

static void model_106cx_init(device_t cpu)
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

	/* TODO: PIC thermal sensor control */

	/* Start up my cpu siblings */
	intel_sibling_init(cpu);
}

static struct device_operations cpu_dev_ops = {
	.init     = model_106cx_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x106c0 }, /* Intel Atom 230 */
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

