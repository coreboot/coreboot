/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef STUMPY_THERMAL_H
#define STUMPY_THERMAL_H

/* PWM when all active fan devices are off */
#define FAN_OFF_PWM		0x00

/* Fan is at LOW speed */
#define FAN3_THRESHOLD_OFF	45
#define FAN3_THRESHOLD_ON	55
#define FAN3_PWM		0xA0

/* Fan is at MEDIUM speed */
#define FAN2_THRESHOLD_OFF	55
#define FAN2_THRESHOLD_ON	68
#define FAN2_PWM		0xC0

/* Fan is at HIGH speed */
#define FAN1_THRESHOLD_OFF	68
#define FAN1_THRESHOLD_ON	80
#define FAN1_PWM		0xE0

/* Fan is at FULL speed */
#define FAN0_THRESHOLD_OFF	80
#define FAN0_THRESHOLD_ON	88
#define FAN0_PWM		0xff

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE	98

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE	90

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE		100

#endif
