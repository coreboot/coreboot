/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef MAINBOARD_SPD_H
#define MAINBOARD_SPD_H

void mainboard_fill_dq_map_ch0(void *dq_map_ptr);
void mainboard_fill_dq_map_ch1(void *dq_map_ptr);
void mainboard_fill_dqs_map_ch0(void *dqs_map_ptr);
void mainboard_fill_dqs_map_ch1(void *dqs_map_ptr);
void mainboard_fill_rcomp_res_data(void *rcomp_ptr);
void mainboard_fill_rcomp_strength_data(void *rcomp_strength_ptr);
#endif
