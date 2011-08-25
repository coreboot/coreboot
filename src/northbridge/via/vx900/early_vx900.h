/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EARLY_VX900_H
#define EARLY_VX900_H

#include <stdint.h>

#define SMBUS_IO_BASE	0x500
#include <devices/smbus/smbus.h>


#include "raminit.h"

void enable_smbus(void);
void dump_spd_data(void);
void spd_read(u8 addr, spd_raw_data spd);

#endif /* EARLY_VX900_H */
