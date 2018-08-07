/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google, Inc.
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

#ifndef __MAINBOARD_GOOGLE_URARA_URARA_BOARDID_H__
#define __MAINBOARD_GOOGLE_URARA_URARA_BOARDID_H__

/*
 * List of URARA derivatives board ID definitions. They are stored in uint8_t
 * across the code, using #defines here not to imply any specific size.
 */
#define URARA_BOARD_ID_BUB	0
#define URARA_BOARD_ID_BURANKU	1
#define URARA_BOARD_ID_DERWENT	2
#define URARA_BOARD_ID_JAGUAR	3
#define URARA_BOARD_ID_KENNET	4
#define URARA_BOARD_ID_SPACE	5

struct board_hw {
	uint8_t i2c_interface;
};

const struct board_hw *board_get_hw(void);

#endif
