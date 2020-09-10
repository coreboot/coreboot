/* SPDX-License-Identifier: GPL-2.0-only */

#define DPTF_TSR0_SENSOR_ID	0
#define DPTF_TSR0_SENSOR_NAME	"TMP432_Internal"
#define DPTF_TSR0_PASSIVE	46
#define DPTF_TSR0_CRITICAL	60

#define DPTF_TSR1_SENSOR_ID	1
#define DPTF_TSR1_SENSOR_NAME	"TMP432_CPU_bottom"
#define DPTF_TSR1_PASSIVE	48
#define DPTF_TSR1_CRITICAL	70

#define DPTF_TSR2_SENSOR_ID	2
#define DPTF_TSR2_SENSOR_NAME	"TMP432_Power_top"
#define DPTF_TSR2_PASSIVE	68
#define DPTF_TSR2_CRITICAL	80

#define DPTF_ENABLE_CHARGER

/* Charger performance states, board-specific values from charger and EC */
Name (CHPS, Package () {
	Package () { 0, 0, 0, 0, 255, 0x6a4, "mA", 0 },	/* 1.7A (MAX) */
	Package () { 0, 0, 0, 0, 24, 0x600, "mA", 0 },	/* 1.5A */
	Package () { 0, 0, 0, 0, 16, 0x400, "mA", 0 },	/* 1.0A */
	Package () { 0, 0, 0, 0, 8, 0x200, "mA", 0 },	/* 0.5A */
	Package () { 0, 0, 0, 0, 0, 0x000, "mA", 0 },	/* 0.0A */
})

/* Mainboard specific _PDL is 1GHz */
Name (MPDL, 8)

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.B0DB, \_SB.PCI0.B0DB, 100, 50, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 0 */
	Package () { \_SB.PCI0.B0DB, \_SB.DPTF.TSR0, 100, 500, 0, 0, 0, 0 },
#ifdef DPTF_ENABLE_CHARGER
	/* Charger Effect on Temp Sensor 1 */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR2, 200, 1200, 0, 0, 0, 0 },
#endif

	/* CPU Effect on Temp Sensor 1 */
	Package () { \_SB.PCI0.B0DB, \_SB.DPTF.TSR1, 100, 550, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 2 */
	Package () { \_SB.PCI0.B0DB, \_SB.DPTF.TSR2, 100, 600, 0, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		2000,	/* PowerLimitMinimum */
		5000,	/* PowerLimitMaximum */
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
