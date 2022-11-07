/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/name.h>
#include <cpu/intel/smm_reloc.h>

#define MSR_BBL_CR_CTL3		0x11e

static void configure_c_states(const int quad)
{
	msr_t msr;

	/* Is C5 requested and supported? */
	const int c5 = southbridge_support_c5() &&
			(rdmsr(MSR_BBL_CR_CTL3).lo & (3 << 30)) &&
			!(rdmsr(MSR_FSB_FREQ).lo & (1 << 31));
	/* Is C6 requested and supported? */
	const int c6 = southbridge_support_c6() &&
			((cpuid_edx(5) >> (6 * 4)) & 0xf) && c5;

	const int cst_range = (c6 ? 6 : (c5 ? 5 : 4)) - 2; /* zero means lvl2 */

	msr = rdmsr(MSR_PKG_CST_CONFIG_CONTROL);
	msr.lo &= ~(1 << 9); // Issue a  single stop grant cycle upon stpclk
	msr.lo |=  (1 << 8);
	if (quad)
		msr.lo = (msr.lo & ~(7 << 0)) | (4 << 0);
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
	wrmsr(MSR_PKG_CST_CONFIG_CONTROL, msr);

	/* Set Processor MWAIT IO BASE */
	msr.hi = 0;
	msr.lo = ((PMB0_BASE + 4) & 0xffff) | (((PMB1_BASE + 9) & 0xffff)
		<< 16);
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

	msr = rdmsr(MSR_EXTENDED_CONFIG);
	/* Super LFM supported? */
	if (northbridge_support_slfm() && (msr.lo & (1 << 27)))
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

	msr = rdmsr(MSR_PKG_CST_CONFIG_CONTROL);
	msr.lo &= ~(1 << 11); /* Enable hw coordination. */
	msr.lo |= (1 << 15); /* Lock config until next reset. */
	wrmsr(MSR_PKG_CST_CONFIG_CONTROL, msr);
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

#define IA32_PECI_CTL		0x5a0

static void configure_misc(const int eist, const int tm2, const int emttm)
{
	msr_t msr;

	const u32 sub_cstates = cpuid_edx(5);

	msr = rdmsr(IA32_MISC_ENABLE);
	msr.lo |= (1 << 3);	/* TM1 enable */
	if (tm2)
		msr.lo |= (1 << 13);	/* TM2 enable */
	msr.lo |= (1 << 17);	/* Bidirectional PROCHOT# */
	msr.lo |= (1 << 18);	/* MONITOR/MWAIT enable */

	msr.lo |= (1 << 10);	/* FERR# multiplexing */

	if (eist)
		msr.lo |= (1 << 16);	/* Enhanced SpeedStep Enable */

	/* Enable C2E */
	if (((sub_cstates >> (2 * 4)) & 0xf) >= 2)
		msr.lo |= (1 << 26);

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

	wrmsr(IA32_MISC_ENABLE, msr);

	if (eist) {
		msr.lo |= (1 << 20);	/* Lock Enhanced SpeedStep Enable */
		wrmsr(IA32_MISC_ENABLE, msr);
	}

	/* Enable PECI
	   WARNING: due to Erratum AW67 described in Intel document #318733
	   the microcode must be updated before this MSR is written to. */
	msr = rdmsr(IA32_PECI_CTL);
	msr.lo |= 1;
	wrmsr(IA32_PECI_CTL, msr);
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

static void model_1067x_init(struct device *cpu)
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

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

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
}

static struct device_operations cpu_dev_ops = {
	.init     = model_1067x_init,
};

static const struct cpu_device_id cpu_table[] = {
	{ X86_VENDOR_INTEL, 0x10676 },
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
