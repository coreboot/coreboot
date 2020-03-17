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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define DPTF_CPU_PASSIVE	98
#define DPTF_CPU_CRITICAL	108

/* Skin Sensor for CPU VR temperature monitor */
#define DPTF_TSR0_SENSOR_ID	1
#define DPTF_TSR0_SENSOR_NAME	"Skin"
#define DPTF_TSR0_PASSIVE	55
#define DPTF_TSR0_CRITICAL	100

/* Memory Sensor for DDR temperature monitor */
#define DPTF_TSR1_SENSOR_ID	2
#define DPTF_TSR1_SENSOR_NAME	"DDR"
#define DPTF_TSR1_PASSIVE	53
#define DPTF_TSR1_CRITICAL	100

/* M.2 Sensor for Ambient temperature monitor */
#define DPTF_TSR2_SENSOR_ID	3
#define DPTF_TSR2_SENSOR_NAME	"Ambient"
#define DPTF_TSR2_PASSIVE	38
#define DPTF_TSR2_CRITICAL	93

#undef DPTF_ENABLE_FAN_CONTROL
#undef DPTF_ENABLE_CHARGER

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.TCPU, \_SB.PCI0.TCPU, 500, 100, 0, 0, 0, 0 },

	/* CPU Throttle Effect on Skin (TSR0) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR0, 400, 40, 0, 0, 0, 0 },

	/* CPU Throttle Effect on DDR (TSR1) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR1, 300, 50, 2, 0, 0, 0 },

	/* CPU Throttle Effect on Ambient (TSR2) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR2, 1000, 100, 1, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		3000,	/* PowerLimitMinimum */
		21000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		28000,	/* TimeWindowMaximum */
		100	/* StepSize */
	},
	Package () {	/* Power Limit 2 */
		1,	/* PowerLimitIndex, 1 for Power Limit 2 */
		15000,	/* PowerLimitMinimum */
		51000,	/* PowerLimitMaximum */
		28000,	/* TimeWindowMinimum */
		28000,	/* TimeWindowMaximum */
		100	/* StepSize */
	}
})
