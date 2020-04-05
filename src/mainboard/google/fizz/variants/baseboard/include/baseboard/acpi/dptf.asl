/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#define DPTF_CPU_PASSIVE	93
#define DPTF_CPU_CRITICAL	100
#define DPTF_CPU_ACTIVE_AC0	90
#define DPTF_CPU_ACTIVE_AC1	77

#define DPTF_TSR0_SENSOR_ID	0
#define DPTF_TSR0_SENSOR_NAME	"TMP432_Internal"
#define DPTF_TSR0_PASSIVE	70
#define DPTF_TSR0_CRITICAL	83
#define DPTF_TSR0_ACTIVE_AC0	95
#define DPTF_TSR0_ACTIVE_AC1	85
#define DPTF_TSR0_ACTIVE_AC2	60
#define DPTF_TSR0_ACTIVE_AC3	52
#define DPTF_TSR0_ACTIVE_AC4	44
#define DPTF_TSR0_ACTIVE_AC5	38
#define DPTF_TSR0_ACTIVE_AC6	35

#define DPTF_TSR1_SENSOR_ID	1
#define DPTF_TSR1_SENSOR_NAME	"TMP432_CPU_bottom"
#define DPTF_TSR1_PASSIVE	67
#define DPTF_TSR1_CRITICAL	73

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.B0D4, \_SB.PCI0.B0D4, 100, 50, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 0 */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR0, 100, 100, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 1 */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR1, 100, 300, 0, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		3000,	/* PowerLimitMinimum */
		15000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		200	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		44000,	/* PowerLimitMinimum */
		44000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		1000	/* StepSize */
	}
})

/* Include DPTF */
#include <soc/intel/skylake/acpi/dptf/dptf.asl>
