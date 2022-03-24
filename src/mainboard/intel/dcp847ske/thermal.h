/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DCP847SKE_THERMAL_H
#define DCP847SKE_THERMAL_H

/* TODO: These should be generated at runtime from
 * MSR_TEMPERATURE_TARGET (0x1a2) */

/* Temperature which OS will shutdown at (Tjmax) */
#define CRITICAL_TEMPERATURE	100

/* Temperature which OS will throttle CPU (Tcontrol) */
#define PASSIVE_TEMPERATURE	86

#endif
