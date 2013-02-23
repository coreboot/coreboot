/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 University of Heidelberg
 * Written by Mondrian Nuessle <nuessle@uni-heidelberg.de> for Univ. Heidelberg
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

#ifndef SUPERIO_SERVERENGINES_PILOT_PILOT_H
#define SUPERIO_SERVERENGINES_PILOT_PILOT_H

/* PILOT Super I/O is only based on LPC observation done on factory system. */

#define PILOT_LD1 0x01 /* Logical device 1 */
#define PILOT_SP1 0x02 /* Com1 */
#define PILOT_LD4 0x04 /* Logical device 4 */
#define PILOT_LD5 0x05 /* Logical device 5 */
#define PILOT_LD7 0x07 /* Logical device 7 */

#endif
