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

#ifndef _MAINBOARD_COMMON_BOARD_ID_H_
#define _MAINBOARD_COMMON_BOARD_ID_H_

#include <stdint.h>

/* Board/FAB ID Command */
#define EC_FAB_ID_CMD	0x0D

/* TGL-U Board IDs */
#define TGL_UP3_LP4_SAMSUNG	0x3
#define TGL_UP3_LP4_HYNIX	0xB
#define TGL_UP3_LP4_MICRON	0x13

/* TGL-Y Board IDs */
#define TGL_UP4_LP4_SAMSUNG	0x5
#define TGL_UP4_LP4_HYNIX	0xD
#define TGL_UP4_LP4_MICRON	0x15

/*
 * Returns board information (board id[15:8] and
 * Fab info[7:0]) on success and < 0 on error
 */
int get_board_id(void);

#endif /* _MAINBOARD_COMMON_BOARD_ID_H_ */
