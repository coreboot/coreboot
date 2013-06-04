/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#ifndef RAMINIT_H
#define RAMINIT_H

#define DIMM_SOCKETS 2

struct mem_controller {
	uint16_t channel0[DIMM_SOCKETS];
};

void sdram_set_registers(const struct mem_controller *ctrl);
void sdram_set_spd_registers(const struct mem_controller *ctrl);
void sdram_enable(int controllers, const struct mem_controller *ctrl);

void sdram_initialize(int controllers, const struct mem_controller *ctrl);

/* mainboard specific */
int spd_read_byte(unsigned device, unsigned address);

#endif
