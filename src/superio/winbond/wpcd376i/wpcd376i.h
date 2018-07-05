/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
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

#ifndef SUPERIO_WINBOND_WPCD376I_WPCD376I_H
#define SUPERIO_WINBOND_WPCD376I_WPCD376I_H

#include <arch/io.h>

/* Logical Device Numbers (LDN). */
#define WPCD376I_FDC	0	/* Floppy */
#define WPCD376I_LPT	1	/* Parallel port */
#define WPCD376I_IR	2	/* Infrared port */
#define WPCD376I_SP1	3	/* UART1 */
#define WPCD376I_SWC	4	/* System wake-up control */
#define WPCD376I_KBCM	5	/* PS/2 mouse */
#define WPCD376I_KBCK	6	/* PS/2 keyboard */
#define WPCD376I_GPIO	7	/* General Purpose I/O */

void wpcd376i_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif
