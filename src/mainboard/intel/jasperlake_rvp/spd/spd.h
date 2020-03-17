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

#ifndef MAINBOARD_SPD_H
#define MAINBOARD_SPD_H

#include <stdint.h>

#define RCOMP_TARGET_PARAMS	0x5

void mainboard_fill_dq_map_ch0(u8 *dq_map_ptr);
void mainboard_fill_dq_map_ch1(u8 *dq_map_ptr);
void mainboard_fill_dqs_map_ch0(u8 *dqs_map_ptr);
void mainboard_fill_dqs_map_ch1(u8 *dqs_map_ptr);
void mainboard_fill_rcomp_res_data(u16 *rcomp_ptr);
void mainboard_fill_rcomp_strength_data(u16 *rcomp_strength_ptr);
#endif
