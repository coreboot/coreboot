/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef THERMAL_H
#define THERMAL_H

#define TEMPERATURE_SENSOR_ID		0	/* PECI */

/* Fan is at default speed */
#define FAN4_PWM		0x4d

/* Fan is at LOW speed */
#define FAN3_THRESHOLD_OFF	40
#define FAN3_THRESHOLD_ON	50
#define FAN3_PWM		0x55

/* Fan is at MEDIUM speed */
#define FAN2_THRESHOLD_OFF	55
#define FAN2_THRESHOLD_ON	67
#define FAN2_PWM		0xa6

/* Fan is at HIGH speed */
#define FAN1_THRESHOLD_OFF	65
#define FAN1_THRESHOLD_ON	70
#define FAN1_PWM		0xc0

/* Fan is at FULL speed */
#define FAN0_THRESHOLD_OFF	90
#define FAN0_THRESHOLD_ON	100
#define FAN0_PWM		0xff

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE	104

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE	95

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE		105

#endif
