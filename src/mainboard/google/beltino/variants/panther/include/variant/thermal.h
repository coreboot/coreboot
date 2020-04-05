/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef THERMAL_H
#define THERMAL_H

/* Fan is OFF */
#define FAN4_THRESHOLD_OFF	0
#define FAN4_THRESHOLD_ON	0
#define FAN4_PWM		0x00

/* Fan is at LOW speed */
#define FAN3_THRESHOLD_OFF	40
#define FAN3_THRESHOLD_ON	50
#define FAN3_PWM		0x6b

/* Fan is at MEDIUM speed */
#define FAN2_THRESHOLD_OFF	75
#define FAN2_THRESHOLD_ON	83
#define FAN2_PWM		0xcc

/* Fan is at HIGH speed */
#define FAN1_THRESHOLD_OFF	86
#define FAN1_THRESHOLD_ON	90
#define FAN1_PWM		0xe5

/* Fan is at FULL speed */
#define FAN0_THRESHOLD_OFF	93
#define FAN0_THRESHOLD_ON	96
#define FAN0_PWM		0xff

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE	100

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE	90

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE		100

#endif
