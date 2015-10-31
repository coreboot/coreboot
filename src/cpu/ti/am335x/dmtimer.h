/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
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

#ifndef __CPU_TI_AM335X_DMTIMER_H__
#define __CPU_TI_AM335X_DMTIMER_H__

#include <stdint.h>

#define OSC_HZ 24000000

void dmtimer_start(int num);
uint64_t dmtimer_raw_value(int num);

#endif
