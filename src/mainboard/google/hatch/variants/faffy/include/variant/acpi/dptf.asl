/* SPDX-License-Identifier: GPL-2.0-only */

#define DPTF_CPU_PASSIVE	90
#define DPTF_CPU_CRITICAL	100

#define DPTF_TSR0_SENSOR_ID	0
#define DPTF_TSR0_SENSOR_NAME	"Thermal Sensor 1"
#define DPTF_TSR0_PASSIVE	60
#define DPTF_TSR0_CRITICAL	85

#define DPTF_TSR1_SENSOR_ID	1
#define DPTF_TSR1_SENSOR_NAME	"Thermal Sensor 2"
#define DPTF_TSR1_PASSIVE	60
#define DPTF_TSR1_CRITICAL	85

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
	Package () {90,		0xFFFFFFFF,	6700,	220,	2200},
	Package () {80,		0xFFFFFFFF,	5800,	180,	1800},
	Package () {70,		0xFFFFFFFF,	5000,	145,	1450},
	Package () {60,		0xFFFFFFFF,	4900,	115,	1150},
	Package () {50,		0xFFFFFFFF,	3838,	90,	900},
	Package () {40,		0xFFFFFFFF,	2904,	55,	550},
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
		\_SB.DPTF.TFN1, \_SB.PCI0.TCPU, 95, 85, 75, 65, 55, 45, 0, 0,
			0, 0, 0
	},
	Package () {
		\_SB.DPTF.TFN1, \_SB.DPTF.TSR0, 95, 85, 75, 65, 55, 45, 0, 0,
			0, 0, 0
	},
	Package () {
		\_SB.DPTF.TFN1, \_SB.DPTF.TSR1, 95, 85, 75, 65, 55, 45, 0, 0,
			0, 0, 0
	}
})

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.TCPU, \_SB.PCI0.TCPU, 100, 50, 0, 0, 0, 0 },

	/* CPU Throttle Effect on Ambient (TSR0) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR0, 100, 60, 0, 0, 0, 0 },

	/* Charger Throttle Effect on Charger (TSR1) */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR1, 100, 60, 0, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		10000,	/* PowerLimitMinimum */
		15000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		32000,	/* TimeWindowMaximum */
		200	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		25000,	/* PowerLimitMinimum */
		51000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		32000,	/* TimeWindowMaximum */
		1000	/* StepSize */
	}
})
