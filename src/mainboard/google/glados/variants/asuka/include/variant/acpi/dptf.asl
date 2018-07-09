/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation
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

#define DPTF_CPU_PASSIVE	101
#define DPTF_CPU_CRITICAL	106

#define DPTF_TSR0_SENSOR_ID	0
#define DPTF_TSR0_SENSOR_NAME	"TMP432_Memory"
#define DPTF_TSR0_PASSIVE	74
#define DPTF_TSR0_CRITICAL	79
#define DPTF_TSR0_ACTIVE_AC0	120
#define DPTF_TSR0_ACTIVE_AC1	110
#define DPTF_TSR0_ACTIVE_AC2	47
#define DPTF_TSR0_ACTIVE_AC3	44
#define DPTF_TSR0_ACTIVE_AC4	41
#define DPTF_TSR0_ACTIVE_AC5	38
#define DPTF_TSR0_ACTIVE_AC6	35

#define DPTF_TSR1_SENSOR_ID	1
#define DPTF_TSR1_SENSOR_NAME	"TMP432_PCH"
#define DPTF_TSR1_PASSIVE	72
#define DPTF_TSR1_CRITICAL	77
#define DPTF_TSR1_ACTIVE_AC0	45
#define DPTF_TSR1_ACTIVE_AC1	42
#define DPTF_TSR1_ACTIVE_AC2	39
#define DPTF_TSR1_ACTIVE_AC3	36
#define DPTF_TSR1_ACTIVE_AC4	33

#define DPTF_TSR2_SENSOR_ID	2
#define DPTF_TSR2_SENSOR_NAME	"TMP432_Battery"
#define DPTF_TSR2_PASSIVE	67
#define DPTF_TSR2_CRITICAL	72

#define DPTF_ENABLE_CHARGER
#define DPTF_ENABLE_FAN_CONTROL
#define DPTF_ENABLE_FAN_CONTROL_TSR1

/* Charger performance states, board-specific values from charger and EC */
Name (CHPS, Package () {
	Package () { 0, 0, 0, 0, 255, 0x6a4, "mA", 0 },	/* 1.7A (MAX) */
	Package () { 0, 0, 0, 0, 24, 0x600, "mA", 0 },	/* 1.5A */
	Package () { 0, 0, 0, 0, 16, 0x400, "mA", 0 },	/* 1.0A */
	Package () { 0, 0, 0, 0, 8, 0x200, "mA", 0 },	/* 0.5A */
	Package () { 0, 0, 0, 0, 0, 0x000, "mA", 0 },	/* 0.0A */
})

#ifdef DPTF_ENABLE_FAN_CONTROL
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
	Package () {
		/*
		 * Source, Target, Weight, AC0, AC1, AC2, AC3, AC4, AC5, AC6,
		 *	AC7, AC8, AC9
		 */
		\_SB.DPTF.TFN1, \_SB.PCI0.B0D4, 100, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0
	},
	Package () {
		\_SB.DPTF.TFN1, \_SB.DPTF.TSR0, 100, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0
	},
	Package () {
		\_SB.DPTF.TFN1, \_SB.DPTF.TSR1, 100, 79, 66, 50, 41, 39, 0,
			0, 0, 0, 0
	}
})
#endif

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.B0D4, \_SB.PCI0.B0D4, 100, 10, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 0 */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR0, 100, 10, 0, 0, 0, 0 },

#ifdef DPTF_ENABLE_CHARGER
	/* Charger Effect on Temp Sensor 1 */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR1, 200, 10, 0, 0, 0, 0 },
#endif

	/* CPU Effect on Temp Sensor 1 */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR1, 100, 10, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 2 */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR2, 100, 10, 0, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		3000,	/* PowerLimitMinimum */
		15000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		28000,	/* TimeWindowMaximum */
		100	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		25000,	/* PowerLimitMinimum */
		25000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		100	/* StepSize */
	}
})
