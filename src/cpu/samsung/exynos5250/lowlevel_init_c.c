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

/*
 * FIXME: This file is essentially the "bootblock" leftover from U-Boot. For
 * now it serves as a reference until all the resume-related stuff is added
 * to the appropriate bootblock/romstage/ramstage files in coreboot.
 */

#include <common.h>
#include <config.h>
#include <cpu/samsung/exynos5-common/exynos5-common.h>
#include <cpu/samsung/exynos5-common/spl.h>
#include <cpu/samsung/exynos5250/clock_init.h>
#include <cpu/samsung/exynos5250/cpu.h>
#include <cpu/samsung/exynos5250/dmc.h>
#include <cpu/samsung/exynos5250/pinmux.h>
#include <cpu/samsung/exynos5250/power.h>
#include <cpu/samsung/exynos5250/setup.h>
#include <cpu/samsung/exynos5250/tzpc.h>
#include "setup.h"

#include <console/console.h>

void do_barriers(void);	/* FIXME: make gcc shut up about "no previous prototype" */

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
//	uint32_t reset_status;
	int actions = 0;

// 	do_barriers();

	/* Setup cpu info which is needed to select correct register offsets */
	cpu_info_init();

#if 0
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
#endif

	actions = DO_UART | DO_CLOCKS | DO_POWER;
	if (actions & DO_POWER)
		power_init();
	if (actions & DO_CLOCKS)
		system_clock_init();
	if (actions & DO_UART) {

		/* Set up serial UART so we can printf() */
		/* FIXME(dhendrix): add a function for mapping
		   CONFIG_CONSOLE_SERIAL_UART_ADDRESS to PERIPH_ID_UARTn */
//		exynos_pinmux_config(EXYNOS_UART, PINMUX_FLAG_NONE);
		exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);

		console_init();
		while (1) {
			console_tx_byte('C');
		}
	}
	init_timer();	/* FIXME(dhendrix): was timer_init() */

#if 0
	if (actions & DO_CLOCKS) {
		mem_ctrl_init();
		tzpc_init();
	}
#endif

//	return actions & DO_WAKEUP;
	return 0;
}
