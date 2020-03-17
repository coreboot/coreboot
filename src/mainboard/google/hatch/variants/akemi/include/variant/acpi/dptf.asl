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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#define DPTF_CPU_PASSIVE	95
#define DPTF_CPU_CRITICAL	105
#define DPTF_CPU_ACTIVE_AC0	87
#define DPTF_CPU_ACTIVE_AC1	85
#define DPTF_CPU_ACTIVE_AC2	83
#define DPTF_CPU_ACTIVE_AC3	80
#define DPTF_CPU_ACTIVE_AC4	75

#define DPTF_TSR0_SENSOR_ID	0
#define DPTF_TSR0_SENSOR_NAME	"Thermal Sensor 1"
#define DPTF_TSR0_PASSIVE	65
#define DPTF_TSR0_CRITICAL	75
#define DPTF_TSR0_ACTIVE_AC0	61
#define DPTF_TSR0_ACTIVE_AC1	59
#define DPTF_TSR0_ACTIVE_AC2	57
#define DPTF_TSR0_ACTIVE_AC3	55
#define DPTF_TSR0_ACTIVE_AC4	51
#define DPTF_TSR0_ACTIVE_AC5	48
#define DPTF_TSR0_ACTIVE_AC6	40

#define DPTF_TSR1_SENSOR_ID	1
#define DPTF_TSR1_SENSOR_NAME	"Thermal Sensor 2"
#define DPTF_TSR1_PASSIVE	38
#define DPTF_TSR1_CRITICAL	75
#define DPTF_TSR1_ACTIVE_AC0    42
#define DPTF_TSR1_ACTIVE_AC1    40
#define DPTF_TSR1_ACTIVE_AC2    38

#define DPTF_TSR2_SENSOR_ID	2
#define DPTF_TSR2_SENSOR_NAME	"Thermal Sensor - CPU"
#define DPTF_TSR2_PASSIVE	62
#define DPTF_TSR2_CRITICAL	105
#define DPTF_TSR2_ACTIVE_AC0	62
#define DPTF_TSR2_ACTIVE_AC1	60
#define DPTF_TSR2_ACTIVE_AC2	59
#define DPTF_TSR2_ACTIVE_AC3	54
#define DPTF_TSR2_ACTIVE_AC4	51
#define DPTF_TSR2_ACTIVE_AC5	48
#define DPTF_TSR2_ACTIVE_AC6	45

#define DPTF_ENABLE_CHARGER
#define DPTF_ENABLE_FAN_CONTROL

/* Charger performance states, board-specific values from charger and EC */
Name (CHPS, Package () {
	Package () { 0, 0, 0, 0, 255, 0x6a4, "mA", 0 },	/* 1.7A (MAX) */
	Package () { 0, 0, 0, 0, 24, 0x600, "mA", 0 },	/* 1.5A */
	Package () { 0, 0, 0, 0, 16, 0x400, "mA", 0 },	/* 1.0A */
	Package () { 0, 0, 0, 0, 8, 0x200, "mA", 0 },	/* 0.5A */
})

/* DFPS: Fan Performance States */
Name (DFPS, Package () {
	0,	// Revision
	/*
	 * TODO : Need to update this Table after characterization.
	 *	  These are initial reference values.
	 */
	/* Control, Trip Point, Speed, NoiseLevel, Power */
	Package () {85,		0xFFFFFFFF,	5500,	180,	1800},
	Package () {79,		0xFFFFFFFF,	5400,	170,	1700},
	Package () {76,		0xFFFFFFFF,	5300,	165,	1650},
	Package () {73,		0xFFFFFFFF,	5200,	160,	1600},
	Package () {70,		0xFFFFFFFF,	5100,	155,	1550},
	Package () {68,		0xFFFFFFFF,	5000,	150,	1500},
	Package () {65,		0xFFFFFFFF,	4900,	145,	1450},
	Package () {62,		0xFFFFFFFF,	4800,	140,	1400},
	Package () {60,		0xFFFFFFFF,	4700,	135,	1350},
	Package () {58,		0xFFFFFFFF,	4600,	130,	1300},
	Package () {55,		0xFFFFFFFF,	4500,	120,	1200},
	Package () {53,		0xFFFFFFFF,	4400,	110,	1100},
	Package () {51,		0xFFFFFFFF,	4300,	100,	1000},
	Package () {49,		0xFFFFFFFF,	4200,	95,	950},
	Package () {47,		0xFFFFFFFF,	4100,	80,	800},
	Package () {46,		0xFFFFFFFF,	4000,	70,	700},
	Package () {45,		0xFFFFFFFF,	3900,	60,	600},
	Package () {43,		0xFFFFFFFF,	3800,	55,	550},
	Package () {42,		0xFFFFFFFF,	3700,	50,	500},
	Package () {40,		0xFFFFFFFF,	3600,	35,	350},
	Package () {38,		0xFFFFFFFF,	3500,	30,	300},
	Package () {36,		0xFFFFFFFF,	3400,	25,	250},
	Package () {33,		0xFFFFFFFF,	3300,	20,	200},
	Package () {32,		0xFFFFFFFF,	3200,	15,	150},
	Package () {31,		0xFFFFFFFF,	3100,	10,	100},
	Package () {30,		0xFFFFFFFF,	3000,	5,	50},
	Package () {0,		0xFFFFFFFF,	0,	0,	0}
})

Name (DART, Package () {
	/* Fan effect on CPU */
	0,	// Revision
	Package () {
		/*
		 * Source, Target, Weight, AC0, AC1, AC2, AC3, AC4, AC5, AC6,
		 *	AC7, AC8, AC9
		 */
		\_SB.DPTF.TFN1, \_SB.PCI0.TCPU, 100, 90, 69, 56, 46, 36, 0, 0,
			0, 0, 0
	},
	Package () {
		\_SB.DPTF.TFN1, \_SB.DPTF.TSR0, 100, 85, 73, 62, 49, 33, 25, 14,
			0, 0, 0
	},
	Package () {
		\_SB.DPTF.TFN1, \_SB.DPTF.TSR1, 100, 85, 73, 62, 0, 0, 0, 0,
			0, 0, 0
	},
	Package () {
		\_SB.DPTF.TFN1, \_SB.DPTF.TSR2, 100, 85, 73, 65, 53, 33, 25, 14,
			0, 0, 0
	},
})

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.TCPU, \_SB.PCI0.TCPU, 100, 50, 0, 0, 0, 0 },

	/* CPU Throttle Effect on Ambient (TSR0) */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR0, 100, 60, 0, 0, 0, 0 },

	/* Charger Throttle Effect on Charger (TSR1) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR1, 100, 60, 0, 0, 0, 0 },

	/* CPU Throttle Effect on CPU (TSR2) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR2, 100, 60, 0, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		5000,	/* PowerLimitMinimum */
		15000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		32000,	/* TimeWindowMaximum */
		200	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		15000,	/* PowerLimitMinimum */
		64000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		32000,	/* TimeWindowMaximum */
		1000	/* StepSize */
	}
})
