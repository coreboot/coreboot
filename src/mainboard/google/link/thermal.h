/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef LINK_THERMAL_H
#define LINK_THERMAL_H

/* Config TDP Sensor ID */
#define CTDP_SENSOR_ID			9 /* PECI */

/* Config TDP Nominal */
#define CTDP_NOMINAL_THRESHOLD_OFF	0
#define CTDP_NOMINAL_THRESHOLD_ON	0

/* Config TDP Down */
#define CTDP_DOWN_THRESHOLD_OFF		80
#define CTDP_DOWN_THRESHOLD_ON		90

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE		104

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE		100

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE			105

#endif
