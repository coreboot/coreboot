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

#define DPTF_CPU_PASSIVE	50
#define DPTF_CPU_CRITICAL	105

#define DPTF_TSR0_SENSOR_ID	0
#define DPTF_TSR0_SENSOR_NAME	"Thermal Sensor - Charger"
#define DPTF_TSR0_PASSIVE	45
#define DPTF_TSR0_CRITICAL	90
#define DPTF_TSR0_TABLET_PASSIVE        32
#define DPTF_TSR0_TABLET_CRITICAL       90

#define DPTF_TSR1_SENSOR_ID	1
#define DPTF_TSR1_SENSOR_NAME	"Thermal Sensor - 5V"
#define DPTF_TSR1_PASSIVE	45
#define DPTF_TSR1_CRITICAL	90
#define DPTF_TSR1_TABLET_PASSIVE        32
#define DPTF_TSR1_TABLET_CRITICAL       90

#define DPTF_TSR2_SENSOR_ID	2
#define DPTF_TSR2_SENSOR_NAME	"Thermal Sensor - IA"
#define DPTF_TSR2_PASSIVE	45
#define DPTF_TSR2_CRITICAL	90
#define DPTF_TSR2_TABLET_PASSIVE        32
#define DPTF_TSR2_TABLET_CRITICAL       90

#define DPTF_TSR3_SENSOR_ID	3
#define DPTF_TSR3_SENSOR_NAME	"Thermal Sensor - GT"
#define DPTF_TSR3_PASSIVE	45
#define DPTF_TSR3_CRITICAL	90
#define DPTF_TSR3_TABLET_PASSIVE        32
#define DPTF_TSR3_TABLET_CRITICAL       90

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
	Package () { \_SB.PCI0.TCPU, \_SB.PCI0.TCPU, 100, 10, 0, 0, 0, 0 },

	/* CPU Throttle Effect on 5V (TSR1) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR1, 100, 10, 0, 0, 0, 0 },

	/* Charger Throttle Effect on Charger (TSR0) */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR0, 100, 94, 0, 0, 0, 0 },

	/* CPU Throttle Effect on IA (TSR2) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR2, 100, 10, 0, 0, 0, 0 },

	/* CPU Throttle Effect on GT (TSR3) */
	Package () { \_SB.PCI0.TCPU, \_SB.DPTF.TSR3, 100, 10, 0, 0, 0, 0 },
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
