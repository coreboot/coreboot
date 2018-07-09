/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 * Copyright (C) 2016 Intel Corporation
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

#define DPTF_CPU_PASSIVE        80
#define DPTF_CPU_CRITICAL       99

#define DPTF_TSR0_SENSOR_ID	1
#define DPTF_TSR0_SENSOR_NAME	"WiFi"
#define DPTF_TSR0_PASSIVE	50
#define DPTF_TSR0_CRITICAL	80
#define DPTF_TSR0_TABLET_PASSIVE	52
#define DPTF_TSR0_TABLET_CRITICAL	80

#define DPTF_TSR1_SENSOR_ID	2
#define DPTF_TSR1_SENSOR_NAME	"PD"
#define DPTF_TSR1_PASSIVE	50
#define DPTF_TSR1_CRITICAL	80
#define DPTF_TSR1_TABLET_PASSIVE	52
#define DPTF_TSR1_TABLET_CRITICAL	80

#define DPTF_TSR2_SENSOR_ID	3
#define DPTF_TSR2_SENSOR_NAME	"DRAM"
#define DPTF_TSR2_PASSIVE	50
#define DPTF_TSR2_CRITICAL	80
#define DPTF_TSR2_TABLET_PASSIVE	52
#define DPTF_TSR2_TABLET_CRITICAL	80

#define DPTF_TSR3_SENSOR_ID	4
#define DPTF_TSR3_SENSOR_NAME	"Charger"
#define DPTF_TSR3_PASSIVE	68
#define DPTF_TSR3_CRITICAL	85
#define DPTF_TSR3_TABLET_PASSIVE	68
#define DPTF_TSR3_TABLET_CRITICAL	85

/* Enable DPTF charger control */
#define DPTF_ENABLE_CHARGER

/* SKL-Y is Fanless design. */
#undef DPTF_ENABLE_FAN_CONTROL

/* Charger performance states, board-specific values from charger and EC */
Name (CHPS, Package () {
	Package () { 0, 0, 0, 0, 255, 0x1338, "mA", 0 },  /* 4920mA (MAX) */
	Package () { 0, 0, 0, 0, 39, 0x9C0, "mA", 0 },	  /* 2496mA */
	Package () { 0, 0, 0, 0, 28, 0x700, "mA", 0 },	  /* 1792mA */
	Package () { 0, 0, 0, 0, 19, 0x4C0, "mA", 0 },	  /* 1216mA */
	Package () { 0, 0, 0, 0, 13, 0x340, "mA", 0 },	  /* 832mA */
	Package () { 0, 0, 0, 0, 10, 0x280, "mA", 0 },	  /* 640mA */
	Package () { 0, 0, 0, 0, 6, 0x180, "mA", 0 },	  /* 384mA */
	Package () { 0, 0, 0, 0, 2, 0x80, "mA", 0 },	  /* 124mA */
})

Name (DTRT, Package () {
	/* CPU Throttle Effect on CPU */
	Package () { \_SB.PCI0.B0D4, \_SB.PCI0.B0D4, 100, 10, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 0 */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR0, 100, 100, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 1 */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR1, 100, 50, 0, 0, 0, 0 },

	/* CPU Effect on Temp Sensor 2 */
	Package () { \_SB.PCI0.B0D4, \_SB.DPTF.TSR2, 100, 100, 0, 0, 0, 0 },

	/* Charger Effect on Temp Sensor 3 */
	Package () { \_SB.DPTF.TCHG, \_SB.DPTF.TSR3, 100, 50, 0, 0, 0, 0 },
})

Name (MPPC, Package ()
{
	0x2,		/* Revision */
	Package () {	/* Power Limit 1 */
		0,	/* PowerLimitIndex, 0 for Power Limit 1 */
		1600,	/* PowerLimitMinimum */
		10000,	/* PowerLimitMaximum */
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
