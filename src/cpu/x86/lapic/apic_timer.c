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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <console/console.h>
#include <delay.h>
#include <thread.h>
#include <arch/io.h>
#include <arch/cpu.h>
#include <arch/early_variables.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/lapic.h>
#include <cpu/intel/speedstep.h>

/* NOTE: This code uses global variables, so it can not be used during
 * memory init.
 */

#if CONFIG_UDELAY_LAPIC_FIXED_FSB
static const u32 timer_fsb = CONFIG_UDELAY_LAPIC_FIXED_FSB;

static int set_timer_fsb(void)
{
	return 0;
}
#else
static u32 timer_fsb CAR_GLOBAL = 0;

static int set_timer_fsb(void)
{
	struct cpuinfo_x86 c;
	int core_fsb[8] = { -1, 133, -1, 166, -1, 100, -1, -1 };
	int core2_fsb[8] = { 266, 133, 200, 166, 333, 100, -1, -1 };

	get_fms(&c, cpuid_eax(1));
	if (c.x86 != 6)
		return -1;

	switch (c.x86_model) {
	case 0xe:  /* Core Solo/Duo */
	case 0x1c: /* Atom */
		timer_fsb = core_fsb[rdmsr(MSR_FSB_FREQ).lo & 7];
		break;
	case 0xf:  /* Core 2 or Xeon */
	case 0x17: /* Enhanced Core */
		timer_fsb = core2_fsb[rdmsr(MSR_FSB_FREQ).lo & 7];
		break;
	case 0x2a: /* SandyBridge BCLK fixed at 100MHz*/
	case 0x3a: /* IvyBridge BCLK fixed at 100MHz*/
	case 0x3c: /* Haswell BCLK fixed at 100MHz */
	case 0x45: /* Haswell-ULT BCLK fixed at 100MHz */
		timer_fsb = 100;
		break;
	default:
		timer_fsb = 200;
		break;
	}

	return 0;
}
#endif

void init_timer(void)
{
	/* Set the apic timer to no interrupts and periodic mode */
	lapic_write(LAPIC_LVTT, (LAPIC_LVT_TIMER_PERIODIC | LAPIC_LVT_MASKED));

	/* Set the divider to 1, no divider */
	lapic_write(LAPIC_TDCR, LAPIC_TDR_DIV_1);

	/* Set the initial counter to 0xffffffff */
	lapic_write(LAPIC_TMICT, 0xffffffff);

	/* Set FSB frequency to a reasonable value */
	set_timer_fsb();
}

void udelay(u32 usecs)
{
	u32 start, value, ticks;

	if (!thread_yield_microseconds(usecs))
		return;

	if (!timer_fsb || (lapic_read(LAPIC_LVTT) &
		(LAPIC_LVT_TIMER_PERIODIC | LAPIC_LVT_MASKED)) !=
		(LAPIC_LVT_TIMER_PERIODIC | LAPIC_LVT_MASKED))
		init_timer();

	/* Calculate the number of ticks to run, our FSB runs at timer_fsb Mhz */
	ticks = usecs * timer_fsb;
	start = lapic_read(LAPIC_TMCCT);
	do {
		value = lapic_read(LAPIC_TMCCT);
	} while((start - value) < ticks);
}

#if CONFIG_LAPIC_MONOTONIC_TIMER && !defined(__PRE_RAM__)
#include <timer.h>

static struct monotonic_counter {
	int initialized;
	struct mono_time time;
	uint32_t last_value;
} mono_counter;

void timer_monotonic_get(struct mono_time *mt)
{
	uint32_t current_tick;
	uint32_t usecs_elapsed;

	if (!mono_counter.initialized) {
		init_timer();
		/* An FSB frequency of 200Mhz provides a 20 second polling
		 * interval between timer_monotonic_get() calls before wrap
		 * around occurs. */
		if (timer_fsb > 200)
			printk(BIOS_WARNING,
			       "apic timer freq (%d) may be too fast.\n",
			       timer_fsb);
		mono_counter.last_value = lapic_read(LAPIC_TMCCT);
		mono_counter.initialized = 1;
	}

	current_tick = lapic_read(LAPIC_TMCCT);
	/* Note that the APIC timer counts down. */
	usecs_elapsed = (mono_counter.last_value - current_tick) / timer_fsb;

	/* Update current time and tick values only if a full tick occurred. */
	if (usecs_elapsed) {
		mono_time_add_usecs(&mono_counter.time, usecs_elapsed);
		mono_counter.last_value = current_tick;
	}

	/* Save result. */
	*mt = mono_counter.time;
}
#endif
