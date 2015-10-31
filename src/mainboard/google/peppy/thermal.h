/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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

/* Control TDP Settings */
#define CTL_TDP_SENSOR_ID		0	/* PECI */
#define CTL_TDP_POWER_LIMIT		12	/* 12W */
#define CTL_TDP_THRESHOLD_OFF		80	/* Normal at 80C */
#define CTL_TDP_THRESHOLD_ON		85	/* Limited at 85C */

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE		99

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE		95

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE			100

#endif
