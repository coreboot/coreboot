/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_SPD_H
#define MAINBOARD_SPD_H

#define SPD_DENSITY_BANKS	4
#define SPD_ADDRESSING		5
#define SPD_ORGANIZATION	7
#define SPD_BUS_DEV_WIDTH	8

int mainboard_get_spd_index(void);
uintptr_t mainboard_get_spd_data(void);

#endif
