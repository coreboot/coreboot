/* SPDX-License-Identifier: GPL-2.0-only */

#define DPTF_CPU_PASSIVE	77
#define DPTF_CPU_CRITICAL	105
#define DPTF_CPU_ACTIVE_AC0	70
#define DPTF_CPU_ACTIVE_AC1	65
#define DPTF_CPU_ACTIVE_AC2	60
#define DPTF_CPU_ACTIVE_AC3	50
#define DPTF_CPU_ACTIVE_AC4	40

#define DPTF_TSR0_SENSOR_ID	0
#define DPTF_TSR0_SENSOR_NAME	"Thermal Sensor 1"
#define DPTF_TSR0_PASSIVE	58
#define DPTF_TSR0_CRITICAL	105
#define DPTF_TSR0_TABLET_PASSIVE	58
#define DPTF_TSR0_TABLET_CRITICAL	105

#define DPTF_TSR1_SENSOR_ID	1
#define DPTF_TSR1_SENSOR_NAME	"Thermal Sensor 2"
#define DPTF_TSR1_PASSIVE	57
#define DPTF_TSR1_CRITICAL	86
#define DPTF_TSR1_TABLET_PASSIVE	49
#define DPTF_TSR1_TABLET_CRITICAL	86

#define DPTF_ENABLE_CHARGER

/* Charger performance states, board-specific values from charger and EC */
Name (CHPS, Package () {
	Package () { 0, 0, 0, 0, 255, 0x6a4, "mA", 0 },	/* 1.7A (MAX) */
	Package () { 0, 0, 0, 0, 24, 0x600, "mA", 0 },	/* 1.5A */
	Package () { 0, 0, 0, 0, 16, 0x400, "mA", 0 },	/* 1.0A */
	Package () { 0, 0, 0, 0, 8, 0x200, "mA", 0 },	/* 0.5A */
})

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.TCPU, \_SB.PCI0.TCPU, 100, 300, 0, 0, 0, 0 },

	/* CPU Throttle Effect on Ambient (TSR1) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR1, 100, 300, 0, 0, 0, 0 },

	/* Charger Throttle Effect on Charger (TSR0) */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR0, 100, 300, 0, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		3000,	/* PowerLimitMinimum */
		15000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		32000,	/* TimeWindowMaximum */
		200	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		51000,	/* PowerLimitMinimum */
		51000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		32000,	/* TimeWindowMaximum */
		1000	/* StepSize */
	}
})
