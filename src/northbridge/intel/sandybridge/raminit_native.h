/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Google Inc.
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

#ifndef RAMINIT_NATIVE_H
#define RAMINIT_NATIVE_H

#include "sandybridge.h"
#include <device/dram/ddr3.h>

/* The order is ch0dimmA, ch0dimmB, ch1dimmA, ch1dimmB.  */
void read_spd(spd_raw_data *spd, u8 addr);
void mainboard_get_spd(spd_raw_data *spd);

#endif				/* RAMINIT_H */
