/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/tsc.h>
#include <console/console.h>
#include <soc/pci_devs.h>
#include <device/pci_ops.h>

unsigned long tsc_freq_mhz(void)
{
	msr_t msr;
	uint8_t cpufid;
	uint8_t cpudid;
	uint8_t boost_states;

	/*
	 * See the Family 15h Models 70h-7Fh BKDG (PID 55072) definition for
	 * MSR0000_0010.  The TSC increments at the P0 frequency. According
	 * to the "Software P-state Numbering" section, P0 is the highest
	 * non-boosted state.  freq = 100MHz * (CpuFid + 10h) / (2^(CpuDid)).
	 */
	boost_states = (pci_read_config32(SOC_PM_DEV, CORE_PERF_BOOST_CTRL)
			>> 2) & 0x7;

	msr = rdmsr(PSTATE_0_MSR + boost_states);
	if (!(msr.hi & 0x80000000))
		die("Unknown error: cannot determine P-state 0\n");

	cpufid = (msr.lo & 0x3f);
	cpudid = (msr.lo & 0x1c0) >> 6;

	return (100 * (cpufid + 0x10)) / (0x01 << cpudid);
}
