/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef THERMAL_H
#define THERMAL_H

#define TEMPERATURE_SENSOR_ID		0	/* PECI */

/* Fan is at default speed */
#define FAN4_PWM		0x4d

/* Fan is at LOW speed */
#define FAN3_THRESHOLD_OFF	42
#define FAN3_THRESHOLD_ON	47
#define FAN3_PWM		0xa5

/* Fan is at MEDIUM speed */
#define FAN2_THRESHOLD_OFF	54
#define FAN2_THRESHOLD_ON	59
#define FAN2_PWM		0xb2

/* Fan is at HIGH speed */
#define FAN1_THRESHOLD_OFF	66
#define FAN1_THRESHOLD_ON	71
#define FAN1_PWM		0xc9

/* Fan is at FULL speed */
#define FAN0_THRESHOLD_OFF	78
#define FAN0_THRESHOLD_ON	83
#define FAN0_PWM		0xd8

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE	100

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE	95

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE		105

#endif
