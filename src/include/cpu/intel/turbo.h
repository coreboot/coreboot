/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _CPU_INTEL_TURBO_H
#define _CPU_INTEL_TURBO_H

#define CPUID_LEAF_PM		6
#define PM_CAP_TURBO_MODE	(1 << 1)

/* Disable the Monitor Mwait FSM feature */
#define MONITOR_MWAIT_DIS_MASK	0x40000

#define H_MISC_DISABLE_TURBO	(1 << 6)

enum {
	TURBO_UNKNOWN,
	TURBO_UNAVAILABLE,
	TURBO_DISABLED,
	TURBO_ENABLED,
};

/* Return current turbo state */
int get_turbo_state(void);

/* Enable turbo */
void enable_turbo(void);

/* Disable turbo */
void disable_turbo(void);

#endif
