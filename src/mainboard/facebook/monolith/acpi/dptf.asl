/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#define DPTF_CPU_PASSIVE        80
#define DPTF_CPU_CRITICAL       90
#define DPTF_CPU_ACTIVE_AC0     90
#define DPTF_CPU_ACTIVE_AC1     80
#define DPTF_CPU_ACTIVE_AC2     70
#define DPTF_CPU_ACTIVE_AC3     60
#define DPTF_CPU_ACTIVE_AC4     50

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.B0D4, \_SB.PCI0.B0D4, 100, 50, 0, 0, 0, 0 },

})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		1600,	/* PowerLimitMinimum */
		15000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		200	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		8000,	/* PowerLimitMinimum */
		8000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		1000	/* StepSize */
	}
})

/* Include DPTF */
#include <soc/intel/skylake/acpi/dptf/dptf.asl>
