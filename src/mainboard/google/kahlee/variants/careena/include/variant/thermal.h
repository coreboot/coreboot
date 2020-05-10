/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef THERMAL_H
#define THERMAL_H

/*
 * Stoney Ridge Thermal Requirements 12 (6W)
 * TDP (W) 6
 * T die,max (°C) 95
 * T ctl,max 85
 * T die,lmt (default) 90
 * T ctl,lmt (default) 80
 */

/* Control TDP Settings */
#define CTL_TDP_SENSOR_ID		2	/* EC TIN2 */

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE		94

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE		85

#endif
