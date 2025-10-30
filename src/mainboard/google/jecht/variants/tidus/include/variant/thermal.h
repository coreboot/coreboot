/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef THERMAL_H
#define THERMAL_H

#define TEMPERATURE_SENSOR_ID		0	/* PECI */

/* Thermal Policy 0 */
/* Fan is at default speed */
#define FAN4_0_PWM		0x4d

/* Fan is at LOW speed */
#define FAN3_0_THRESHOLD_OFF	62
#define FAN3_0_THRESHOLD_ON	70
#define FAN3_0_PWM		0x5e

/* Fan is at MEDIUM speed */
#define FAN2_0_THRESHOLD_OFF	68
#define FAN2_0_THRESHOLD_ON	81
#define FAN2_0_PWM		0x78

/* Fan is at HIGH speed */
#define FAN1_0_THRESHOLD_OFF	78
#define FAN1_0_THRESHOLD_ON	91
#define FAN1_0_PWM		0x93

/* Fan is at FULL speed */
#define FAN0_0_THRESHOLD_OFF	88
#define FAN0_0_THRESHOLD_ON	100
#define FAN0_0_PWM		0xb0

/* Thermal Policy 1 */
/* Fan is at default speed */
#define FAN4_1_PWM		0x4d

/* Fan is at LOW speed */
#define FAN3_1_THRESHOLD_OFF	62
#define FAN3_1_THRESHOLD_ON	70
#define FAN3_1_PWM		0x5e

/* Fan is at MEDIUM speed */
#define FAN2_1_THRESHOLD_OFF	68
#define FAN2_1_THRESHOLD_ON	81
#define FAN2_1_PWM		0x70

/* Fan is at HIGH speed */
#define FAN1_1_THRESHOLD_OFF	78
#define FAN1_1_THRESHOLD_ON	91
#define FAN1_1_PWM		0x83

/* Fan is at FULL speed */
#define FAN0_1_THRESHOLD_OFF	88
#define FAN0_1_THRESHOLD_ON	100
#define FAN0_1_PWM		0x93

/* Thermal Policy 2 */
/* Fan is at default speed */
#define FAN4_2_PWM		0x4d

/* Fan is at LOW speed */
#define FAN3_2_THRESHOLD_OFF	62
#define FAN3_2_THRESHOLD_ON	70
#define FAN3_2_PWM		0x59

/* Fan is at MEDIUM speed */
#define FAN2_2_THRESHOLD_OFF	68
#define FAN2_2_THRESHOLD_ON	81
#define FAN2_2_PWM		0x63

/* Fan is at HIGH speed */
#define FAN1_2_THRESHOLD_OFF	78
#define FAN1_2_THRESHOLD_ON	91
#define FAN1_2_PWM		0x6e

/* Fan is at FULL speed */
#define FAN0_2_THRESHOLD_OFF	88
#define FAN0_2_THRESHOLD_ON	100
#define FAN0_2_PWM		0x7e

/* Threshold to change thermal policy */
#define THERMAL_POLICY_0_THRESHOLD_OFF	38
#define THERMAL_POLICY_0_THRESHOLD_ON	40

#define THERMAL_POLICY_1_THRESHOLD_OFF	33
#define THERMAL_POLICY_1_THRESHOLD_ON	35

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE		103

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE		95

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE			105

#endif
