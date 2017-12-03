/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
 * Copyright (C) 2017 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
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

#ifndef DCP847SKE_THERMAL_H
#define DPC847SKE_THERMAL_H

/* TODO: These should be generated at runtime from
 * MSR_TEMPERATURE_TARGET (0x1a2) */

/* Temperature which OS will shutdown at (Tjmax) */
#define CRITICAL_TEMPERATURE	100

/* Temperature which OS will throttle CPU (Tcontrol) */
#define PASSIVE_TEMPERATURE	86

#endif
