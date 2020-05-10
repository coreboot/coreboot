/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * udelay() implementation for SMI handlers
 * This is neat in that it never writes to hardware registers, and thus does
 *  not modify the state of the hardware while servicing SMIs.
 */

#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/tsc.h>
#include <delay.h>
#include <stdint.h>

void udelay(uint32_t us)
{
	uint8_t fid, did, pstate_idx;
	uint64_t tsc_clock, tsc_start, tsc_now, tsc_wait_ticks;
	msr_t msr;
	const uint64_t tsc_base = 100000000;

	/* Get initial timestamp before we do the math */
	tsc_start = rdtscll();

	/* Get the P-state. This determines which MSR to read */
	msr = rdmsr(PS_STS_REG);
	pstate_idx = msr.lo & 0x07;

	/* Get FID and VID for current P-State */
	msr = rdmsr(PSTATE_0_MSR + pstate_idx);

	/* Extract the FID and VID values */
	fid = msr.lo & 0x3f;
	did = (msr.lo >> 6) & 0x7;

	/* Calculate the CPU clock (from base freq of 100MHz) */
	tsc_clock = tsc_base * (fid + 0x10) / (1 << did);

	/* Now go on and wait */
	tsc_wait_ticks = (tsc_clock / 1000000) * us;

	do {
		tsc_now = rdtscll();
	} while (tsc_now - tsc_wait_ticks < tsc_start);
}
