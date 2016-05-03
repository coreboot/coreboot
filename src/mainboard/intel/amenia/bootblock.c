/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
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

#include <bootblock_common.h>
#include <soc/lpc.h>

void bootblock_mainboard_init(void)
{
	/* Configure pads so that our signals make it out of the SOC. */
	lpc_configure_pads();

	/* Ports 62/66, 60/64, and 200->208 are needed by ChromeEC */
	lpc_enable_fixed_io_ranges(IOE_EC_62_66 | IOE_KBC_60_64 | IOE_LGE_200);

	/* Ports 800 -> 9ff are used by ChromeEC. */
	lpc_open_pmio_window(0x800, 0x200);
}
