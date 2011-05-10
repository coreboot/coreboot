/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>
#include <delay.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>

/* NOTE: This code uses global variables, so it can not be used during
 * memory init.
 */

#define FSB_CLOCK_STS 0xcd

static u32 timer_fsb = 200; // default to 200MHz

void init_timer(void)
{
	msr_t fsb_clock_sts;

	/* Set the apic timer to no interrupts and periodic mode */
	lapic_write(LAPIC_LVTT, (LAPIC_LVT_TIMER_PERIODIC | LAPIC_LVT_MASKED));

	/* Set the divider to 1, no divider */
	lapic_write(LAPIC_TDCR, LAPIC_TDR_DIV_1);

	/* Set the initial counter to 0xffffffff */
	lapic_write(LAPIC_TMICT, 0xffffffff);

	/* Set FSB frequency to a reasonable value */
	fsb_clock_sts = rdmsr(FSB_CLOCK_STS);
	switch ((fsb_clock_sts.lo >> 4) & 0x07) {
	case 0: timer_fsb = 266; break;
	case 1: timer_fsb = 133; break;
	case 2: timer_fsb = 200; break;
	case 3: timer_fsb = 166; break;
	case 5: timer_fsb = 100; break;
	}
}

void udelay(u32 usecs)
{
	u32 start, value, ticks;
	/* Calculate the number of ticks to run, our FSB runs at timer_fsb Mhz */
	ticks = usecs * timer_fsb;
	start = lapic_read(LAPIC_TMCCT);
	do {
		value = lapic_read(LAPIC_TMCCT);
	} while((start - value) < ticks);
}
