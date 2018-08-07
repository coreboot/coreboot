/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2018 Intel Corporation.
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

#ifndef MAINBOARD_SPD_H
#define MAINBOARD_SPD_H

#define RCOMP_TARGET_PARAMS	0x5

void mainboard_fill_dq_map_ch0(void *dq_map_ptr);
void mainboard_fill_dq_map_ch1(void *dq_map_ptr);
void mainboard_fill_dqs_map_ch0(void *dqs_map_ptr);
void mainboard_fill_dqs_map_ch1(void *dqs_map_ptr);
void mainboard_fill_rcomp_res_data(void *rcomp_ptr);
void mainboard_fill_rcomp_strength_data(void *rcomp_strength_ptr);
#endif
