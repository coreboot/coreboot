/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
 * Copyright (C) 2004 Tyan
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

#ifndef SUPERIO_SMSC_DME1737_H
#define SUPERIO_SMSC_DME1737_H

#define DME1737_FDC		0	/* Floppy */
#define DME1737_PP		3	/* Parallel Port */
#define DME1737_SP1		4	/* Com1 */
#define DME1737_SP2		5	/* Com2 */
#define DME1737_KBC		7	/* Keyboard & Mouse */
#define DME1737_RT		10	/* Runtime reg*/

#include <arch/io.h>
#include <stdint.h>

void dme1737_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif /* SUPERIO_SMSC_DME1737_H */
