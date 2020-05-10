/* SPDX-License-Identifier: GPL-2.0-only */

#define DPTF_CPU_PASSIVE	90
#define DPTF_CPU_CRITICAL	105

#define DPTF_TSR0_SENSOR_ID	0
#define DPTF_TSR0_SENSOR_NAME	"Thermal Sensor - Charger"
#define DPTF_TSR0_PASSIVE	75
#define DPTF_TSR0_CRITICAL	90

#define DPTF_TSR1_SENSOR_ID	1
#define DPTF_TSR1_SENSOR_NAME	"Thermal Sensor - 5V"
#define DPTF_TSR1_PASSIVE	70
#define DPTF_TSR1_CRITICAL	90
#define DPTF_TSR1_ACTIVE_AC0	48
#define DPTF_TSR1_ACTIVE_AC1	46
#define DPTF_TSR1_ACTIVE_AC2	44
#define DPTF_TSR1_ACTIVE_AC3	41
#define DPTF_TSR1_ACTIVE_AC4	39

#define DPTF_TSR2_SENSOR_ID	2
#define DPTF_TSR2_SENSOR_NAME	"Thermal Sensor - GT"
#define DPTF_TSR2_PASSIVE	75
#define DPTF_TSR2_CRITICAL	90

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
	0,      // Revision
	/* Control, Trip Point, Speed, NoiseLevel, Power */
	Package () {90,         0xFFFFFFFF,     5900,   220,    2200},
	Package () {80,         0xFFFFFFFF,     5400,   180,    1800},
	Package () {70,         0xFFFFFFFF,     4900,   145,    1450},
	Package () {60,         0xFFFFFFFF,     4500,   115,    1150},
	Package () {50,         0xFFFFFFFF,     4000,   90,     900},
	Package () {40,         0xFFFFFFFF,     3000,   55,     550},
	Package () {30,         0xFFFFFFFF,     2200,   30,     300},
	Package () {20,         0xFFFFFFFF,     1600,   15,     150},
	Package () {10,         0xFFFFFFFF,     800,    10,     100},
	Package () {0,          0xFFFFFFFF,     0,      0,      50}
})

Name (DART, Package () {
	0,      // Revision

	/*
	 * Source, Target, Weight, AC0, AC1, AC2, AC3, AC4, AC5, AC6, AC7, AC8, AC9
	 */
	Package () {
		\_SB.DPTF.TFN1, \_SB.PCI0.TCPU, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},
	Package () {
		\_SB.DPTF.TFN1, \_SB.DPTF.TSR0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},
	Package () {
		\_SB.DPTF.TFN1, \_SB.DPTF.TSR1, 100, 70, 63, 54, 48, 44, 0, 0, 0, 0, 0
	},
	Package () {
		\_SB.DPTF.TFN1, \_SB.DPTF.TSR2, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	}
})

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.TCPU, \_SB.PCI0.TCPU, 100, 10, 0, 0, 0, 0 },

	/* CPU Throttle Effect on 5V (TSR1) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR1, 100, 10, 0, 0, 0, 0 },

	/* Charger Throttle Effect on Charger (TSR0) */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR0, 100, 94, 0, 0, 0, 0 },

	/* CPU Throttle Effect on GT (TSR2) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR2, 100, 10, 0, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		7000,	/* PowerLimitMinimum */
		9000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		28000,	/* TimeWindowMaximum */
		250	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		51000,	/* PowerLimitMinimum */
		51000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		28000,	/* TimeWindowMaximum */
		1000	/* StepSize */
	}
})
