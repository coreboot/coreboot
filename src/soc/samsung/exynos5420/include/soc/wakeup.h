/*
 * This file is part of the coreboot project.
 *
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

#ifndef CPU_SAMSUNG_EXYNOS5420_WAKEUP_H
#define CPU_SAMSUNG_EXYNOS5420_WAKEUP_H

/* Power Down Modes */
#define S5P_CHECK_SLEEP	0x00000BAD
#define S5P_CHECK_DIDLE	0xBAD00000
#define S5P_CHECK_LPA	0xABAD0000

enum {
	// A normal boot (not suspend/resume)
	IS_NOT_WAKEUP,
	// A wake up event that can be resumed any time
	WAKEUP_DIRECT,
	// A wake up event that must be resumed only after
	// clock and memory controllers are re-initialized
	WAKEUP_NEED_CLOCK_RESET,
};

int wakeup_need_reset(void);
int get_wakeup_state(void);
void wakeup(void);
void wakeup_enable_uart(void);

#endif	/* CPU_SAMSUNG_EXYNOS5420_WAKEUP_H */
