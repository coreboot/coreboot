/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Arastra, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef NORTHBRIDGE_INTEL_I3100_RAMINIT_EP80579_H
#define NORTHBRIDGE_INTEL_I3100_RAMINIT_EP80579_H

#define DIMM_SOCKETS 2
struct mem_controller {
	u32 node_id;
	device_t f0;
	u16 channel0[DIMM_SOCKETS];
};

void sdram_initialize(int controllers, const struct mem_controller *ctrl);
#endif
