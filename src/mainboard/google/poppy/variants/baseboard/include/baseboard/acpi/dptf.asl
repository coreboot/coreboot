/* SPDX-License-Identifier: GPL-2.0-only */

#define DPTF_CPU_PASSIVE	94
#define DPTF_CPU_CRITICAL	99

#define DPTF_TSR0_SENSOR_ID	1
#define DPTF_TSR0_SENSOR_NAME	"Ambient"
#define DPTF_TSR0_PASSIVE	55
#define DPTF_TSR0_CRITICAL	70

#define DPTF_TSR1_SENSOR_ID	2
#define DPTF_TSR1_SENSOR_NAME	"Charger"
#define DPTF_TSR1_PASSIVE	55
#define DPTF_TSR1_CRITICAL	75

#define DPTF_TSR2_SENSOR_ID	3
#define DPTF_TSR2_SENSOR_NAME	"DRAM"
#define DPTF_TSR2_PASSIVE	52
#define DPTF_TSR2_CRITICAL	75

#define DPTF_TSR3_SENSOR_ID	4
#define DPTF_TSR3_SENSOR_NAME	"eMMC"
#define DPTF_TSR3_PASSIVE	55
#define DPTF_TSR3_CRITICAL	75

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
	Package () { \_SB.PCI0.B0D4, \_SB.PCI0.B0D4, 100, 50, 0, 0, 0, 0 },

	/* CPU Throttle Effect on Ambient (TSR0) */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR0, 100, 600, 0, 0, 0, 0 },

#ifdef DPTF_ENABLE_CHARGER
	/* Charger Throttle Effect on Charger (TSR1) */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR1, 100, 600, 0, 0, 0, 0 },
#endif

	/* CPU Throttle Effect on DRAM (TSR2) */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR2, 100, 90, 0, 0, 0, 0 },

	/* CPU Throttle Effect on eMMC (TSR3) */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR3, 100, 600, 0, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		1600,	/* PowerLimitMinimum */
		4500,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
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
