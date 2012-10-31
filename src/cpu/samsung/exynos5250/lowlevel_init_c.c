/*
 * Lowlevel setup for SMDK5250 board based on S5PC520
 *
 * Copyright (C) 2012 Samsung Electronics
 * Copyright (c) 2012 The Chromium OS Authors.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <config.h>
#include <asm/arch-exynos/spl.h>
#include <asm/arch/cpu.h>
#include <asm/arch/dmc.h>
#include <asm/arch/pinmux.h>
#include <asm/arch/power.h>
#include <asm/arch/tzpc.h>
#include "setup.h"

void do_barriers(void)
{
	/*
	 * The reason we don't write out the instructions dsb/isb/sev:
	 * While ARM Cortex-A8 supports ARM v7 instruction set (-march=armv7a),
	 * we compile with -march=armv5 to allow more compilers to work.
	 * For U-Boot code this has no performance impact.
	 */
	__asm__ __volatile__(
#if defined(__thumb__)
	".hword 0xF3BF, 0x8F4F\n"  /* dsb; darn -march=armv5 */
	".hword 0xF3BF, 0x8F6F\n"  /* isb; darn -march=armv5 */
	".hword 0xBF40\n"          /* sev; darn -march=armv5 */
#else
	".word  0xF57FF04F\n"      /* dsb; darn -march=armv5 */
	".word  0xF57FF06F\n"      /* isb; darn -march=armv5 */
	".word  0xE320F004\n"      /* sev; darn -march=armv5 */
#endif
	);
}

/* These are the things we can do during low-level init */
enum {
	DO_WAKEUP	= 1 << 0,
	DO_UART		= 1 << 1,
	DO_CLOCKS	= 1 << 2,
	DO_POWER	= 1 << 3,
};

int lowlevel_init_subsystems(void)
{
	uint32_t reset_status;
	int actions = 0;

	do_barriers();

	/* Setup cpu info which is needed to select correct register offsets */
	cpu_info_init();

	reset_status = power_read_reset_status();

	switch (reset_status) {
	case S5P_CHECK_SLEEP:
		actions = DO_CLOCKS | DO_WAKEUP;
		break;
	case S5P_CHECK_DIDLE:
	case S5P_CHECK_LPA:
		actions = DO_WAKEUP;
	default:
		/* This is a normal boot (not a wake from sleep) */
		actions = DO_UART | DO_CLOCKS | DO_POWER;
	}

	if (actions & DO_POWER)
		power_init();
	if (actions & DO_CLOCKS)
		system_clock_init();
	if (actions & DO_UART) {
		/* Set up serial UART so we can printf() */
		exynos_pinmux_config(EXYNOS_UART, PINMUX_FLAG_NONE);
		serial_init();
		timer_init();
	}
	if (actions & DO_CLOCKS) {
		mem_ctrl_init();
		tzpc_init();
	}

	return actions & DO_WAKEUP;
}
