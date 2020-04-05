/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#define DPTF_CPU_PASSIVE		80
#define DPTF_CPU_CRITICAL		105

#define DPTF_TSR0_SENSOR_ID		1
#define DPTF_TSR0_SENSOR_NAME		"Ambient"
#define DPTF_TSR0_PASSIVE		50
#define DPTF_TSR0_CRITICAL		75

#define DPTF_TSR1_SENSOR_ID		2
#define DPTF_TSR1_SENSOR_NAME		"Charger"
#define DPTF_TSR1_PASSIVE		57
#define DPTF_TSR1_CRITICAL		75

#define DPTF_TSR2_SENSOR_ID		3
#define DPTF_TSR2_SENSOR_NAME		"DRAM"
#define DPTF_TSR2_PASSIVE		53
#define DPTF_TSR2_CRITICAL		75

#define DPTF_TSR3_SENSOR_ID		4
#define DPTF_TSR3_SENSOR_NAME		"eMMC"
#define DPTF_TSR3_PASSIVE		50
#define DPTF_TSR3_CRITICAL		75

#undef DPTF_ENABLE_FAN_CONTROL
#define DPTF_ENABLE_CHARGER

/* Charger performance states, board-specific values from charger and EC */
Name (CHPS, Package () {
	Package () { 0, 0, 0, 0, 255, 0xbb8, "mA", 0 },	/* 3000mA (MAX) */
	Package () { 0, 0, 0, 0, 24, 0x800, "mA", 0 },	/* 2000mA */
	Package () { 0, 0, 0, 0, 16, 0x400, "mA", 0 },	/* 1000mA */
	Package () { 0, 0, 0, 0, 8, 0x200, "mA", 0 },	/* 500mA */
})

Name (DTRT, Package () {
	/* CPU Throttle Effect on Ambient */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR0, 100, 200, 0, 0, 0, 0 },

	/* CPU Throttle Effect on DRAM */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR2, 100, 200, 0, 0, 0, 0 },

	/* CPU Throttle Effect on eMMC */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR3, 100, 200, 0, 0, 0, 0 },

	/* Charger Throttle Effect on Ambient */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR0, 200, 200, 0, 0, 0, 0 },

	/* Charger Throttle Effect on Charger */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR1, 200, 200, 0, 0, 0, 0 },

	/* Charger Throttle Effect on DRAM */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR2, 200, 200, 0, 0, 0, 0 },

	/* Charger Throttle Effect on eMMC */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR3, 200, 200, 0, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		3000,	/* PowerLimitMinimum */
		7000,	/* PowerLimitMaximum */
		5000,	/* TimeWindowMinimum */
		5000,	/* TimeWindowMaximum */
		200	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		15000,	/* PowerLimitMinimum */
		15000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		1000	/* StepSize */
	}
})

/* Include DPTF */
#include <soc/intel/skylake/acpi/dptf/dptf.asl>
