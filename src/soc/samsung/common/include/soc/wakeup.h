/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_SAMSUNG_COMMON_INCLUDE_SOC_WAKEUP_H
#define SOC_SAMSUNG_COMMON_INCLUDE_SOC_WAKEUP_H

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

#endif	/* SOC_SAMSUNG_COMMON_INCLUDE_SOC_WAKEUP_H */
