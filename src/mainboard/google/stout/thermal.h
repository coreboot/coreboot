/* SPDX-License-Identifier: GPL-2.0-only */

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
