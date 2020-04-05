/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#define DPTF_TSR0_SENSOR_ID	1
#define DPTF_TSR0_SENSOR_NAME	"TMP432_Internal"
#define DPTF_TSR0_PASSIVE	40
#define DPTF_TSR0_CRITICAL	70

#define DPTF_TSR1_SENSOR_ID	2
#define DPTF_TSR1_SENSOR_NAME	"TMP432_Power_top"
#define DPTF_TSR1_PASSIVE	45
#define DPTF_TSR1_CRITICAL	70

#define DPTF_TSR2_SENSOR_ID	3
#define DPTF_TSR2_SENSOR_NAME	"TMP432_CPU_bottom"
#define DPTF_TSR2_PASSIVE	35
#define DPTF_TSR2_CRITICAL	70

#undef DPTF_ENABLE_CHARGER
