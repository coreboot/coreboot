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

#ifndef _MAINBOARD_BOARD_ID_H_
#define _MAINBOARD_BOARD_ID_H_

#include <stdint.h>

/* Mobile Board Id 0x00 - 0xFF */
#define BOARD_ID_SKL_A0_RVP3	0x04
#define BOARD_ID_SKL_RVP7	0x0B
#define BOARD_ID_KBL_RVP8	0x42
#define BOARD_ID_KBL_RVP11	0x44

/* 60-6F reserved for KBL RVPs */
#define BOARD_ID_KBL_LPDDR3_RVP3	0x60
#define BOARD_ID_KBL_LPDDR3_RVP7	0x64

/* Board/FAB ID Command */
#define EC_FAB_ID_CMD	0x0D

/*
 * Returns board information (board id[15:8] and
 * Fab info[7:0]) on success and < 0 on error
 */
int get_ec_boardinfo(void);

/* Return spd index */
int get_spd_index(u8 *spd_index);

/* Board id[15:8] */
int get_board_id(void);

#endif /* _MAINBOARD_BOARD_ID_H_ */
