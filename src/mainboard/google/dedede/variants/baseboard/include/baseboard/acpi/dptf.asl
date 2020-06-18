/* SPDX-License-Identifier: GPL-2.0-only */

/* Below values might change after Thermal Tuning. */
#define DPTF_CPU_PASSIVE	90
#define DPTF_CPU_CRITICAL	99

#define DPTF_TSR0_SENSOR_ID	0
#define DPTF_TSR0_SENSOR_NAME	"Memory"
#define DPTF_TSR0_PASSIVE	80
#define DPTF_TSR0_CRITICAL	90

#define DPTF_TSR1_SENSOR_ID	1
#define DPTF_TSR1_SENSOR_NAME	"Ambient"
#define DPTF_TSR1_PASSIVE	55
#define DPTF_TSR1_CRITICAL	80

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.TCPU, \_SB.PCI0.TCPU, 100, 100, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 0 */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR0, 100, 600, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 1 */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR1, 100, 150, 0, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		3000,	/* PowerLimitMinimum */
		6000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		200	/* StepSize */
	},

	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		6000,	/* PowerLimitMinimum */
		20000,	/* PowerLimitMaximum */
		1000,	/* TimeWindowMinimum */
		1000,	/* TimeWindowMaximum */
		1000	/* StepSize */
	}
})
