/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#define DPTF_CPU_PASSIVE	80
#define DPTF_CPU_CRITICAL	90

#define DPTF_TSR0_SENSOR_ID	1
#define DPTF_TSR0_SENSOR_NAME	"TMP432_Power_Bottom"
#define DPTF_TSR0_PASSIVE	75
#define DPTF_TSR0_CRITICAL	80

#define DPTF_TSR1_SENSOR_ID	2
#define DPTF_TSR1_SENSOR_NAME	"TMP432_RAM_bottom"
#define DPTF_TSR1_PASSIVE	75
#define DPTF_TSR1_CRITICAL	80

#define DPTF_TSR2_SENSOR_ID	3
#define DPTF_TSR2_SENSOR_NAME	"TMP432_CPU_bottom"
#define DPTF_TSR2_PASSIVE	75
#define DPTF_TSR2_CRITICAL	80

#define DPTF_ENABLE_CHARGER

/* Charger performance states, board-specific values from charger and EC */
Name (CHPS, Package () {
	Package () { 0, 0, 0, 0, 255, 0x8ca, "mA", 0 }, /* 2.25A (MAX) */
	Package () { 0, 0, 0, 0, 32, 0x7d0, "mA", 0 },  /* 2.0A */
	Package () { 0, 0, 0, 0, 24, 0x600, "mA", 0 },  /* 1.5A */
	Package () { 0, 0, 0, 0, 16, 0x400, "mA", 0 },  /* 1.0A */
	Package () { 0, 0, 0, 0, 8, 0x200, "mA", 0 },   /* 0.5A */
	Package () { 0, 0, 0, 0, 0, 0x080, "mA", 0 },   /* 0.128A */
})
