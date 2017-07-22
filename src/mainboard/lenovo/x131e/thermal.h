/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
 * Copyright (C) 2017 James Ye <jye836@gmail.com>
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

#ifndef X131E_THERMAL_H
#define X131E_THERMAL_H

/* Active Thermal and fans are controlled by the EC. */

/* Temperature which OS will shutdown at */
#define CRITICAL_TEMPERATURE    100

/* Temperature which OS will throttle CPU */
#define PASSIVE_TEMPERATURE     90

#endif /* X131E_THERMAL_H */
