/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/tsc.h>
#include <console/console.h>
#include <soc/pci_devs.h>
#include <soc/msr.h>
#include <device/pci_ops.h>

unsigned long tsc_freq_mhz(void)
{
	union pstate_msr pstate_reg;

	/*
	 * See the Family 15h Models 70h-7Fh BKDG (PID 55072) definition for
	 * MSR0000_0010.  The TSC increments at the P0 frequency. According
	 * to the "Software P-state Numbering" section, P0 is the highest
	 * non-boosted state.  freq = 100MHz * (CpuFid + 10h) / (2^(CpuDid)).
	 */
	pstate_reg.raw = rdmsr(PSTATE_MSR(get_pstate_0_reg())).raw;
	if (!pstate_reg.pstate_en)
		die("Unknown error: cannot determine P-state 0\n");

	return get_pstate_core_freq(pstate_reg);
}
