/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef _MAINBOARD_THERMAL_H_
#define _MAINBOARD_THERMAL_H_

#define TEMPERATURE_SENSOR_ID		0	/* PECI */

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE		104

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE		95

/* Tj_max value for calculating PECI CPU temperature */
/* Tj_max can be read by MSR 0x1A2, BITS[23:16] */
#define MAX_TEMPERATURE			100

#endif /* _MAINBOARD_THERMAL_H_ */
