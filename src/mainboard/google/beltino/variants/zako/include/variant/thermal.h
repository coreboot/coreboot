/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef THERMAL_H
#define THERMAL_H

/* Fan is OFF */
#define FAN4_THRESHOLD_OFF	0
#define FAN4_THRESHOLD_ON	0
#define FAN4_PWM		0x4c

/* Fan is at LOW speed */
#define FAN3_THRESHOLD_OFF	48
#define FAN3_THRESHOLD_ON	52
#define FAN3_PWM		0x6d

/* Fan is at MEDIUM speed */
#define FAN2_THRESHOLD_OFF	50
#define FAN2_THRESHOLD_ON	55
#define FAN2_PWM		0x7c

/* Fan is at HIGH speed */
#define FAN1_THRESHOLD_OFF	52
#define FAN1_THRESHOLD_ON	58
#define FAN1_PWM		0xa3

/* Fan is at FULL speed */
#define FAN0_THRESHOLD_OFF	55
#define FAN0_THRESHOLD_ON	60
#define FAN0_PWM		0xba

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE	98

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE	90

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE		100

#endif
