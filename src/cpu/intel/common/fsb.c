/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/intel/fsb.h>
#include <cpu/intel/speedstep.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/tsc.h>
#include <delay.h>
#include <stdint.h>

static u32 timer_fsb;
static u32 timer_tsc;

/* This is not an architectural MSR. */
#define MSR_PLATFORM_INFO 0xce

static int get_fsb_tsc(int *fsb, int *ratio)
{
	struct cpuinfo_x86 c;
	static const short core_fsb[8] = { -1, 133, -1, 166, -1, 100, -1, -1 };
	static const short core2_fsb[8] = { 266, 133, 200, 166, 333, 100, 400, -1 };
	static const short f2x_fsb[8] = { 100, 133, 200, 166, 333, -1, -1, -1 };
	static const short rangeley_fsb[4] = { 83, 100, 133, 116 };
	msr_t msr;

	get_fms(&c, cpuid_eax(1));
	switch (c.x86) {
	case 0x6:
		switch (c.x86_model) {
		case 0xe:  /* Core Solo/Duo */
		case 0x1c: /* Atom */
			*fsb = core_fsb[rdmsr(MSR_FSB_FREQ).lo & 7];
			*ratio = (rdmsr(IA32_PERF_STATUS).hi >> 8) & 0x1f;
			break;
		case 0xf:  /* Core 2 or Xeon */
		case 0x17: /* Enhanced Core */
			*fsb = core2_fsb[rdmsr(MSR_FSB_FREQ).lo & 7];
			*ratio = (rdmsr(IA32_PERF_STATUS).hi >> 8) & 0x1f;
			break;
		case 0x25: /* Arrandale BCLK fixed at 133MHz */
			*fsb = 133;
			*ratio = (rdmsr(MSR_PLATFORM_INFO).lo >> 8) & 0xff;
			break;
		case 0x2a: /* SandyBridge BCLK fixed at 100MHz */
		case 0x3a: /* IvyBridge BCLK fixed at 100MHz */
		case 0x3c: /* Haswell BCLK fixed at 100MHz */
		case 0x3d: /* Broadwell-ULT BCLK fixed at 100MHz */
		case 0x45: /* Haswell-ULT BCLK fixed at 100MHz */
		case 0x46: /* Haswell-GT3e BCLK fixed at 100MHz */
		case 0x47: /* Broadwell BCLK fixed at 100MHz */
			*fsb = 100;
			*ratio = (rdmsr(MSR_PLATFORM_INFO).lo >> 8) & 0xff;
			break;
		case 0x4d: /* Rangeley */
			*fsb = rangeley_fsb[rdmsr(MSR_FSB_FREQ).lo & 3];
			*ratio = (rdmsr(MSR_PLATFORM_INFO).lo >> 8) & 0xff;
			break;
		default:
			return -2;
		}
		break;
	case 0xf: /* Netburst */
		msr = rdmsr(MSR_EBC_FREQUENCY_ID);
		*ratio = msr.lo >> 24;
		switch (c.x86_model) {
		case 0x2:
			*fsb = f2x_fsb[(msr.lo >> 16) & 7];
			break;
		case 0x3:
		case 0x4:
		case 0x6:
			*fsb = core2_fsb[(msr.lo >> 16) & 7];
			break;
		default:
			return -2;
		}
		break;
	default:
		return -2;
	}
	if (*fsb > 0)
		return 0;
	return -1;
}

static void resolve_timebase(void)
{
	int ret, fsb, ratio;

	ret = get_fsb_tsc(&fsb, &ratio);
	if (ret == 0) {
		u32 tsc = 100 * DIV_ROUND_CLOSEST(ratio * fsb, 100);
		timer_fsb = fsb;
		timer_tsc = tsc;
		return;
	}

	if (ret == -1)
		printk(BIOS_ERR, "FSB not found\n");
	if (ret == -2)
		printk(BIOS_ERR, "CPU not supported\n");

	/* Set some semi-ridiculous defaults. */
	timer_fsb = 500;
	timer_tsc = 5000;
}

u32 get_timer_fsb(void)
{
	if (timer_fsb > 0)
		return timer_fsb;

	resolve_timebase();
	return timer_fsb;
}

unsigned long tsc_freq_mhz(void)
{
	if (timer_tsc > 0)
		return timer_tsc;

	resolve_timebase();
	return timer_tsc;
}

/**
 * @brief Returns three times the FSB clock in MHz
 *
 * The result of calculations with the returned value shall be divided by 3.
 * This helps to avoid rounding errors.
 */
int get_ia32_fsb_x3(void)
{
	const int fsb = get_timer_fsb();

	if (fsb > 0)
		return 100 * DIV_ROUND_CLOSEST(3 * fsb, 100);

	printk(BIOS_ERR, "FSB not supported or not found\n");
	return -1;
}
