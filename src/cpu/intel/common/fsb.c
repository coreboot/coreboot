/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include <cpu/intel/fsb.h>
#include <console/console.h>
#include <commonlib/helpers.h>

static int get_fsb(void)
{
	struct cpuinfo_x86 c;
	static const short core_fsb[8] = { -1, 133, -1, 166, -1, 100, -1, -1 };
	static const short core2_fsb[8] = { 266, 133, 200, 166, 333, 100, 400, -1 };
	static const short f2x_fsb[8] = { 100, 133, 200, 166, 333, -1, -1, -1 };
	msr_t msr;
	int ret = -2;

	get_fms(&c, cpuid_eax(1));
	switch (c.x86) {
	case 0x6:
		switch (c.x86_model) {
		case 0xe:  /* Core Solo/Duo */
		case 0x1c: /* Atom */
			ret = core_fsb[rdmsr(MSR_FSB_FREQ).lo & 7];
			break;
		case 0xf:  /* Core 2 or Xeon */
		case 0x17: /* Enhanced Core */
			ret = core2_fsb[rdmsr(MSR_FSB_FREQ).lo & 7];
			break;
		case 0x25: /* Nehalem BCLK fixed at 133MHz */
			ret = 133;
			break;
		case 0x2a: /* SandyBridge BCLK fixed at 100MHz */
		case 0x3a: /* IvyBridge BCLK fixed at 100MHz */
		case 0x3c: /* Haswell BCLK fixed at 100MHz */
		case 0x45: /* Haswell-ULT BCLK fixed at 100MHz */
			ret = 100;
			break;
		}
		break;
	case 0xf: /* Netburst */
		msr = rdmsr(MSR_EBC_FREQUENCY_ID);
		switch (c.x86_model) {
		case 0x2:
			ret = f2x_fsb[(msr.lo >> 16) & 7];
			break;
		case 0x3:
		case 0x4:
		case 0x6:
			ret = core2_fsb[(msr.lo >> 16) & 7];
			break;
		}
	}
	return ret;
}

int get_ia32_fsb(void)
{
	int ret;

	ret = get_fsb();
	if (ret == -1)
		printk(BIOS_ERR, "FSB not found\n");
	if (ret == -2)
		printk(BIOS_ERR, "CPU not supported\n");
	return ret;
}

/**
 * @brief Returns three times the FSB clock in MHz
 *
 * The result of calculations with the returned value shall be divided by 3.
 * This helps to avoid rounding errors.
 */
int get_ia32_fsb_x3(void)
{
	const int fsb = get_ia32_fsb();

	if (fsb > 0)
		return 100 * DIV_ROUND_CLOSEST(3 * fsb, 100);

	printk(BIOS_ERR, "FSB not supported or not found\n");
	return -1;
}
