/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef STUMPY_THERMAL_H
#define STUMPY_THERMAL_H

/* Fan is OFF at idle (passive cooling) */
#define FAN4_THRESHOLD_OFF	0
#define FAN4_THRESHOLD_ON	0
#define FAN4_PWM		0x00

/* Fan is at LOW speed */
#define FAN3_THRESHOLD_OFF	45
#define FAN3_THRESHOLD_ON	55
#define FAN3_PWM		0xA0

/* Fan is at MEDIUM speed */
#define FAN2_THRESHOLD_OFF	55
#define FAN2_THRESHOLD_ON	65
#define FAN2_PWM		0xB0

/* Fan is at HIGH speed */
#define FAN1_THRESHOLD_OFF	65
#define FAN1_THRESHOLD_ON	72
#define FAN1_PWM		0xC0

/* Fan is at FULL speed */
#define FAN0_THRESHOLD_OFF	72
#define FAN0_THRESHOLD_ON	80
#define FAN0_PWM		0xff

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE	98

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE	90

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE		100

#endif
