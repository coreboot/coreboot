/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef THERMAL_H
#define THERMAL_H

#define TEMPERATURE_SENSOR_ID		0	/* PECI */

/* Power level to set when EC requests throttle */
#define EC_THROTTLE_POWER_LIMIT		12	/* 12W */

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE		99

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE		95

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE			100

#endif
