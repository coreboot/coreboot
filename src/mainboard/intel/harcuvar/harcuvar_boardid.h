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
 *
 */

#ifndef HARCUVAR_MAINBOARD_BOARD_H
#define HARCUVAR_MAINBOARD_BOARD_H

#include <stdint.h>

#define BoardIdHarcuvar 0x52

uint8_t board_id(void);

#endif /* MAINBOARD_BOARD_H */
