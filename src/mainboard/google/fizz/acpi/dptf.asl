/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Google Inc.
 * Copyright (C) 2017 Intel Corporation.
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

#define DPTF_CPU_PASSIVE	85
#define DPTF_CPU_CRITICAL	99
#define DPTF_CPU_ACTIVE_AC0	90
#define DPTF_CPU_ACTIVE_AC1	77

#define DPTF_TSR0_SENSOR_ID	0
#define DPTF_TSR0_SENSOR_NAME	"TMP432_Internal"
#define DPTF_TSR0_PASSIVE	66
#define DPTF_TSR0_CRITICAL	71
#define DPTF_TSR0_ACTIVE_AC0	95
#define DPTF_TSR0_ACTIVE_AC1	85
#define DPTF_TSR0_ACTIVE_AC2	60
#define DPTF_TSR0_ACTIVE_AC3	52
#define DPTF_TSR0_ACTIVE_AC4	44
#define DPTF_TSR0_ACTIVE_AC5	38
#define DPTF_TSR0_ACTIVE_AC6	35

#define DPTF_TSR1_SENSOR_ID	1
#define DPTF_TSR1_SENSOR_NAME	"TMP432_CPU_bottom"
#define DPTF_TSR1_PASSIVE	65
#define DPTF_TSR1_CRITICAL	70

#define DPTF_ENABLE_FAN_CONTROL

/* DFPS: Fan Performance States */
Name (DFPS, Package () {
	0,	// Revision
	/*
	 * TODO : Need to update this Table after characterization.
	 *	  These are initial reference values.
	 */
	/* Control, Trip Point, Speed, NoiseLevel, Power */
	Package () {100,	0xFFFFFFFF,	4986,	220,	2200},
	Package () {90,		0xFFFFFFFF,	4804,	180,	1800},
	Package () {80,		0xFFFFFFFF,	4512,	145,	1450},
	Package () {70,		0xFFFFFFFF,	4204,	115,	1150},
	Package () {60,		0xFFFFFFFF,	3838,	90,	900},
	Package () {50,		0xFFFFFFFF,	3402,	65,	650},
	Package () {40,		0xFFFFFFFF,	2904,	45,	450},
	Package () {30,		0xFFFFFFFF,	2337,	30,	300},
	Package () {20,		0xFFFFFFFF,	1608,	15,	150},
	Package () {10,		0xFFFFFFFF,	800,	10,	100},
	Package () {0,		0xFFFFFFFF,	0,	0,	50}
})

Name (DART, Package () {
	/* Fan effect on CPU */
	0,	// Revision
	/*
	 * Source, Target, Weight, AC0, AC1, AC2, AC3, AC4, AC5, AC6,
	 *	AC7, AC8, AC9
	 */
	Package () {
		\_SB.DPTF.TFN1, \_SB.PCI0.B0D4, 100, 100, 72, 0, 0, 0, 0, 0,
			0, 0, 0
	},
	Package () {
		\_SB.DPTF.TFN1, \_SB.DPTF.TSR0, 100, 100, 72, 68, 49, 39, 38,
			37, 0, 0, 0
	}
})

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.B0D4, \_SB.PCI0.B0D4, 100, 50, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 0 */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR0, 100, 600, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 1 */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR1, 100, 600, 0, 0, 0, 0 },
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
		44000,	/* PowerLimitMinimum */
		44000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		1000	/* StepSize */
	}
})

/* Include DPTF */
#include <soc/intel/skylake/acpi/dptf/dptf.asl>
