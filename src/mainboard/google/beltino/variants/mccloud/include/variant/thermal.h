/*
 * This file is part of the coreboot project.
 *
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

#ifndef THERMAL_H
#define THERMAL_H

/* Fan is OFF */
#define FAN4_THRESHOLD_OFF	0
#define FAN4_THRESHOLD_ON	0
#define FAN4_PWM		0x00

/* Fan is at LOW speed */
#define FAN3_THRESHOLD_OFF	35
#define FAN3_THRESHOLD_ON	40
#define FAN3_PWM		0x88

/* Fan is at MEDIUM speed */
#define FAN2_THRESHOLD_OFF	44
#define FAN2_THRESHOLD_ON	48
#define FAN2_PWM		0x94

/* Fan is at HIGH speed */
#define FAN1_THRESHOLD_OFF	53
#define FAN1_THRESHOLD_ON	58
#define FAN1_PWM		0xb5

/* Fan is at FULL speed */
#define FAN0_THRESHOLD_OFF	65
#define FAN0_THRESHOLD_ON	70
#define FAN0_PWM		0xc4

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE	98

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE	90

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE		100

#endif
