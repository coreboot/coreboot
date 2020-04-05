/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef THERMAL_H
#define THERMAL_H

/* Control TDP Settings */
#define CTL_TDP_SENSOR_ID		0	/* PECI */
#define CTL_TDP_POWER_LIMIT		9	/* 8W */
#define CTL_TDP_THRESHILD_NORMAL	0	/*Normal TDP Threshold*/
#define CTL_TDP_THRESHOLD_OFF		66	/* Normal at 64C */
#define CTL_TDP_THRESHOLD_ON		75	/* Limited at 76C */

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE		104

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE		95

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE			105

#endif
