/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey_osgood@verizon.net>
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
 */

#ifndef RAMINIT_H
#define RAMINIT_H

#define DIMM_SOCKETS 1		/* Only one works, for now. */

struct mem_controller {
	pci_devfn_t d0f0, d0f2, d0f3, d0f4, d0f7, d1f0;
	u8 channel0[DIMM_SOCKETS];
};

void ddr_ram_setup(const struct mem_controller *ctrl);

/* mainboard specific */
int spd_read_byte(unsigned device, unsigned address);

#endif
