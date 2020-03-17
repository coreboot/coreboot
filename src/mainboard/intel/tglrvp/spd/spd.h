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

/* SPD index definition should be matched with the order of SPD_SOURCES */
#define SPD_ID_MICRON		0x0
#define SPD_ID_SAMSUNG		0x1
#define SPD_ID_HYNIX		0x2

void mainboard_fill_dq_map_ch0(void *dq_map_ptr);
void mainboard_fill_dq_map_ch1(void *dq_map_ptr);
void mainboard_fill_dqs_map_ch0(void *dqs_map_ptr);
void mainboard_fill_dqs_map_ch1(void *dqs_map_ptr);
void mainboard_fill_rcomp_res_data(void *rcomp_ptr);
void mainboard_fill_rcomp_strength_data(void *rcomp_strength_ptr);

#endif
