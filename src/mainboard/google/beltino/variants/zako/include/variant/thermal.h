/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef THERMAL_H
#define THERMAL_H

/* Fan is OFF at idle (passive cooling) */
#define FAN4_PWM		0x00

/* Fan is at LOW speed */
#define FAN3_THRESHOLD_OFF	40
#define FAN3_THRESHOLD_ON	50
#define FAN3_PWM		0x6d

/* Fan is at MEDIUM speed */
#define FAN2_THRESHOLD_OFF	55
#define FAN2_THRESHOLD_ON	67
#define FAN2_PWM		0x7c

/* Fan is at HIGH speed */
#define FAN1_THRESHOLD_OFF	67
#define FAN1_THRESHOLD_ON	75
#define FAN1_PWM		0xa3

/* Fan is at FULL speed */
#define FAN0_THRESHOLD_OFF	85
#define FAN0_THRESHOLD_ON	90
#define FAN0_PWM		0xdc

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE	98

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE	90

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE		100

#endif
