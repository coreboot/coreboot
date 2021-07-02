/* SPDX-License-Identifier: GPL-2.0-only */

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
