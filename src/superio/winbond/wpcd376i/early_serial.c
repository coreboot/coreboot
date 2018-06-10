/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Marc Jones <marcj303@gmail.com>
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

/* Pre-RAM driver for the Winbond WPCD376I Super I/O chip. */


#include <arch/io.h>
#include <device/pnp_def.h>
#include "wpcd376i.h"

void wpcd376i_enable_serial(pnp_devfn_t dev, u16 iobase)
{
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, iobase);
	pnp_set_enable(dev, 1);
}
