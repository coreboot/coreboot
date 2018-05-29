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
#include <compiler.h>
#include <arch/acpi.h>

struct chipset_power_state {
	uint32_t prev_sleep_state;
} __packed;

struct chipset_power_state *get_power_state(void);
#if IS_ENABLED(CONFIG_PLATFORM_USES_FSP1_1)
struct chipset_power_state *fill_power_state(void);
#else
int fill_power_state(void);
#endif

#endif /* _SOC_PM_H_ */
