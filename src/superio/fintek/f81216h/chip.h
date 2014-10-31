/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SUPERIO_FINTEK_F81216H_CHIP_H
#define SUPERIO_FINTEK_F81216H_CHIP_H

#include <stdint.h>

/* Member variables are defined in devicetree.cb. */
struct superio_fintek_f81216h_config {
	/**
	 *  KEY1   KEY0   Enter key
	 *  0      0      0x77 (default)
	 *  0      1      0xA0
	 *  1      0      0x87
	 *  1      1      0x67
	 *
	 *  See page 17 of data sheet.
	 */
	uint8_t conf_key_mode;
};

#endif /* SUPERIO_FINTEK_F81216H_CHIP_H */
