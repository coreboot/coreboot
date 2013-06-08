/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *               2012 secunet Security Networks AG
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
#include <cpu/intel/speedstep.h>
#include <cpu/intel/hyperthreading.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>

#include "chip.h"

static const uint32_t microcode_updates[] = {
	#include "microcode-m011067660F.h"
	#include "microcode-m041067660F.h"
	#include "microcode-m101067660F.h"
	#include "microcode-m101067770A.h"
	#include "microcode-m111067AA0B.h"
	#include "microcode-m401067660F.h"
	#include "microcode-m441067AA0B.h"
	#include "microcode-m801067660F.h"
	#include "microcode-mA01067AA0B.h"

	/*  Dummy terminator  */
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0,
};

static void init_timer(void)
{
	/* Set the apic timer to no interrupts and periodic mode */
	lapic_write(LAPIC_LVTT, (1 << 17)|(1<< 16)|(0 << 12)|(0 << 0));

	/* Set the divider to 1, no divider */
	lapic_write(LAPIC_TDCR, LAPIC_TDR_DIV_1);

	/* Set the initial counter to 0xffffffff */
	lapic_write(LAPIC_TMICT, 0xffffffff);
}

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

#define MSR_BBL_CR_CTL3		0x11e

static void configure_c_states(const int quad)
{
	msr_t msr;

	/* Find pointer to CPU configuration. */
	const device_t lapic = dev_find_lapic(SPEEDSTEP_APIC_MAGIC);
	const struct cpu_intel_model_1067x_config *const conf =
		(lapic && lapic->chip_info) ? lapic->chip_info : NULL;

	/* Is C5 requested and supported? */
	const int c5 = conf && conf->c5 &&
			(rdmsr(MSR_BBL_CR_CTL3).lo & (3 << 30)) &&
			!(rdmsr(MSR_FSB_FREQ).lo & (1 << 31));
	/* Is C6 requested and supported? */
	const int c6 = conf && conf->c6 &&
			((cpuid_edx(5) >> (6 * 4)) & 0xf) && c5;

	const int cst_range = (c6 ? 6 : (c5 ? 5 : 4)) - 2; /* zero means lvl2 */

	msr = rdmsr(MSR_PMG_CST_CONFIG_CONTROL);
	msr.lo &= ~(1 << 9); // Issue a  single stop grant cycle upon stpclk
	msr.lo |=  (1 << 8);
	if (quad) {
		msr.lo = (msr.lo & ~(7 << 0)) | (4 << 0);
	}
	if (c5) {
		msr.lo &= ~(1 << 13);
		msr.lo &= ~(7 <<  0);
		msr.lo |= (1 <<  3); /* Enable dynamic L2. */
		msr.lo |= (1 << 14); /* Enable deeper sleep */
	}
	/* Next two fields seem to be mutually exclusive: */
	msr.lo &= ~(7 << 4);
	msr.lo |= (1 << 10); /* Enable IO MWAIT redirection. */
	if (c6)
		msr.lo |= (1 << 25);
	wrmsr(MSR_PMG_CST_CONFIG_CONTROL, msr);

	/* Set Processor MWAIT IO BASE */
	msr.hi = 0;
	msr.lo = ((PMB0_BASE + 4) & 0xffff) | (((PMB1_BASE + 9) & 0xffff) << 16);
	wrmsr(MSR_PMG_IO_BASE_ADDR, msr);

	/* Set IO Capture Address */
	msr.hi = 0;
	msr.lo = ((PMB0_BASE + 4) & 0xffff) | ((cst_range & 0xffff) << 16);
	wrmsr(MSR_PMG_IO_CAPTURE_ADDR, msr);

	if (c5) {
		msr = rdmsr(MSR_BBL_CR_CTL3);
		msr.lo &= ~(7 << 25);
		msr.lo |=  (2 << 25);
		msr.lo &= ~(3 << 30);
		msr.lo |=  (1 << 30);
		wrmsr(MSR_BBL_CR_CTL3, msr);
	}
}

static void configure_p_states(const char stepping, const char cores)
{
	msr_t msr;

	/* Find pointer to CPU configuration. */
	const device_t lapic = dev_find_lapic(SPEEDSTEP_APIC_MAGIC);
	struct cpu_intel_model_1067x_config *const conf =
		(lapic && lapic->chip_info) ? lapic->chip_info : NULL;

	msr = rdmsr(MSR_EXTENDED_CONFIG);
	if (conf->slfm && (msr.lo & (1 << 27))) /* Super LFM supported? */
		msr.lo |= (1 << 28); /* Enable Super LFM. */
	wrmsr(MSR_EXTENDED_CONFIG, msr);

	if (rdmsr(MSR_FSB_CLOCK_VCC).hi & (1 << (63 - 32))) {
							/* Turbo supported? */
		if ((stepping == 0xa) && (cores < 4)) {
			msr = rdmsr(MSR_FSB_FREQ);
			msr.lo |= (1 << 3); /* Enable hysteresis. */
			wrmsr(MSR_FSB_FREQ, msr);
		}
		msr = rdmsr(IA32_PERF_CTL);
		msr.hi &= ~(1 << (32 - 32)); /* Clear turbo disable. */
		wrmsr(IA32_PERF_CTL, msr);
	}

	msr = rdmsr(MSR_PMG_CST_CONFIG_CONTROL);
	msr.lo &= ~(1 << 11); /* Enable hw coordination. */
	msr.lo |= (1 << 15); /* Lock config until next reset. */
	wrmsr(MSR_PMG_CST_CONFIG_CONTROL, msr);
}

#define MSR_EMTTM_CR_TABLE(x)	(0xa8 + (x))
#define MSR_EMTTM_TABLE_NUM	6
static void configure_emttm_tables(void)
{
	int i;
	int num_states, pstate_idx;
	msr_t msr;
	sst_table_t pstates;

	/* Gather p-state information. */
	speedstep_gen_pstates(&pstates);

	/* Never turbo mode or Super LFM. */
	num_states = pstates.num_states;
	if (pstates.states[0].is_turbo)
		--num_states;
	if (pstates.states[pstates.num_states - 1].is_slfm)
		--num_states;
	/* Repeat lowest p-state if we haven't enough states. */
	const int num_lowest_pstate =
		(num_states < MSR_EMTTM_TABLE_NUM)
		? (MSR_EMTTM_TABLE_NUM - num_states) + 1
		: 1;
	/* Start from the lowest entry but skip Super LFM. */
	if (pstates.states[pstates.num_states - 1].is_slfm)
		pstate_idx = pstates.num_states - 2;
	else
		pstate_idx = pstates.num_states - 1;
	for (i = 0; i < MSR_EMTTM_TABLE_NUM; ++i) {
		if (i >= num_lowest_pstate)
			--pstate_idx;
		const sst_state_t *const pstate = &pstates.states[pstate_idx];
		printk(BIOS_DEBUG, "writing P-State %d: %d, %d, "
				   "%2d, 0x%02x, %d; encoded: 0x%04x\n",
			pstate_idx, pstate->dynfsb, pstate->nonint,
			pstate->ratio, pstate->vid, pstate->power,
			SPEEDSTEP_ENCODE_STATE(*pstate));
		msr.hi = 0;
		msr.lo = SPEEDSTEP_ENCODE_STATE(pstates.states[pstate_idx]) &
						/* Don't set half ratios. */
						~SPEEDSTEP_RATIO_NONINT;
		wrmsr(MSR_EMTTM_CR_TABLE(i), msr);
	}

	msr = rdmsr(MSR_EMTTM_CR_TABLE(5));
	msr.lo |= (1 << 31); /* lock tables */
	wrmsr(MSR_EMTTM_CR_TABLE(5), msr);
}

static void configure_misc(const int eist, const int tm2, const int emttm)
{
	msr_t msr;

	const u32 sub_cstates = cpuid_edx(5);

	msr = rdmsr(IA32_MISC_ENABLES);
	msr.lo |= (1 << 3);	/* TM1 enable */
	if (tm2)
		msr.lo |= (1 << 13);	/* TM2 enable */
	msr.lo |= (1 << 17);	/* Bidirectional PROCHOT# */
	msr.lo |= (1 << 18);	/* MONITOR/MWAIT enable */

	msr.lo |= (1 << 10);	/* FERR# multiplexing */

	if (eist)
		msr.lo |= (1 << 16);	/* Enhanced SpeedStep Enable */

	/* Enable C2E */
	if (((sub_cstates >> (2 * 4)) & 0xf) >= 2) {
		msr.lo |= (1 << 26);
	}

	/* Enable C4E */
	if (((sub_cstates >> (4 * 4)) & 0xf) >= 2) {
		msr.hi |= (1 << (32 - 32)); // C4E
		msr.hi |= (1 << (33 - 32)); // Hard C4E
	}

	/* Enable EMTTM */
	if (emttm)
		msr.hi |= (1 << (36 - 32));

	/* Enable turbo mode */
	if (rdmsr(MSR_FSB_CLOCK_VCC).hi & (1 << (63 - 32)))
		msr.hi &= ~(1 << (38 - 32));

	wrmsr(IA32_MISC_ENABLES, msr);

	if (eist) {
		msr.lo |= (1 << 20);	/* Lock Enhanced SpeedStep Enable */
		wrmsr(IA32_MISC_ENABLES, msr);
	}
}

#define PIC_SENS_CFG	0x1aa
static void configure_pic_thermal_sensors(const int tm2, const int quad)
{
	msr_t msr;

	msr = rdmsr(PIC_SENS_CFG);

	if (quad)
		msr.lo |=  (1 << 31);
	else
		msr.lo &= ~(1 << 31);
	if (tm2)
		msr.lo |= (1 << 20); /* Enable TM1 if TM2 fails. */
	msr.lo |= (1 << 21); // inter-core lock TM1
	msr.lo |= (1 << 4); // Enable bypass filter /* What does it do? */

	wrmsr(PIC_SENS_CFG, msr);
}

static void model_1067x_init(device_t cpu)
{
	char processor_name[49];


	/* Gather some information: */

	const struct cpuid_result cpuid1 = cpuid(1);

	/* Read stepping. */
	const char stepping = cpuid1.eax & 0xf;
	/* Read number of cores. */
	const char cores = (cpuid1.ebx >> 16) & 0xf;
	/* Is this a quad core? */
	const char quad = cores > 2;
	/* Is this even a multiprocessor? */
	const char mp = cores > 1;

	/* Enable EMTTM on uni- and on multi-processors if it's not disabled. */
	const char emttm = !mp || !(rdmsr(MSR_EXTENDED_CONFIG).lo & 4);

	/* Is enhanced speedstep supported? */
	const char eist = (cpuid1.ecx & (1 << 7)) &&
			  !(rdmsr(IA32_PLATFORM_ID).lo & (1 << 17));
	/* Test for TM2 only if EIST is available. */
	const char tm2 = eist && (cpuid1.ecx & (1 << 8));


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

	/* Initialize the APIC timer */
	init_timer();

	/* Enable virtualization */
	enable_vmx();

	/* Configure C States */
	configure_c_states(quad);

	/* Configure P States */
	configure_p_states(stepping, cores);

	/* EMTTM */
	if (emttm)
		configure_emttm_tables();

	/* Configure Enhanced SpeedStep and Thermal Sensors */
	configure_misc(eist, tm2, emttm);

	/* PIC thermal sensor control */
	configure_pic_thermal_sensors(tm2, quad);

	/* Start up my cpu siblings */
	intel_sibling_init(cpu);
}

static struct device_operations cpu_dev_ops = {
	.init     = model_1067x_init,
};

static struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x10676 }, /* Intel Core 2 Solo/Core Duo */
	{ X86_VENDOR_INTEL, 0x10677 },
	{ X86_VENDOR_INTEL, 0x1067A },
	{ 0, 0 },
};

static const struct cpu_driver driver __cpu_driver = {
	.ops      = &cpu_dev_ops,
	.id_table = cpu_table,
};

struct chip_operations cpu_intel_model_1067x_ops = {
	CHIP_NAME("Intel Penryn CPU")
};
