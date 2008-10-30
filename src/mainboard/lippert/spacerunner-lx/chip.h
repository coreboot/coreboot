/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 LiPPERT Embedded Computers GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdint.h>

extern struct chip_operations mainboard_lippert_spacerunner_lx_ops;

struct mainboard_lippert_spacerunner_lx_config {
	/* bit2 = RS485_EN2, bit1 = RS485_EN1, bit0 = Live LED */
	u8 sio_gp1x_config;
};
