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
#define WPCD376I_FDC	0x00  /* Floppy */
#define WPCD376I_LPT	0x01  /* Parallel port */
/*			0x02     Undefined */
#define WPCD376I_SP1	0x03  /* UART1 */
#define WPCD376I_SWC	0x04  /* System wake-up control */
#define WPCD376I_KBCM	0x05  /* PS/2 mouse */
#define WPCD376I_KBCK	0x06  /* PS/2 keyboard */
#define WPCD376I_GPIO	0x07  /* General Purpose I/O */
#define WPCD376I_ECIR	0x15  /* Enhanced Consumer Infrared Functions (ECIR) */
#define WPCD376I_IR	0x16  /* UART3 & Infrared port */

void wpcd376i_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif
