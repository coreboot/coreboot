/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2016 Intel Corporation.
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

#ifndef _SOC_PM_H_
#define _SOC_PM_H_

#include <stdint.h>

/* Generic sleep state types */
#define SLEEP_STATE_S0		0
#define SLEEP_STATE_S3		3
#define SLEEP_STATE_S5		5

struct chipset_power_state {
	uint32_t prev_sleep_state;
} __attribute__ ((packed));

struct chipset_power_state *fill_power_state(void);

#endif
