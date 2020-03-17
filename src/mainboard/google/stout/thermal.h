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

#ifndef STOUT_THERMAL_H
#define STOUT_THERMAL_H

/* Active Thermal and fans are controlled by the EC. */

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE    99

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE     90

/* Tj_max value for calculating PECI CPU temperature */
#define MAX_TEMPERATURE        100

#endif /* STOUT_THERMAL_H */
