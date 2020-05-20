/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef THERMAL_H
#define THERMAL_H

/*
 * Picasso Thermal Requirements
 * TDP (W) 15
 * T die,max (°C) 105
 * T ctl,max 105
 * T die,lmt (default) 100
 * T ctl,lmt (default) 100
 */

/* Control TDP Settings */
#define CTL_TDP_SENSOR_ID		2	/* EC TIN2 */

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE		104

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE		95

#endif
