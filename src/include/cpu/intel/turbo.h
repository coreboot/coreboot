/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The ChromiumOS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _CPU_INTEL_TURBO_H
#define _CPU_INTEL_TURBO_H

#define CPUID_LEAF_PM		6
#define PM_CAP_TURBO_MODE	(1 << 1)

#define MSR_IA32_MISC_ENABLES	0x1a0
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

#endif
