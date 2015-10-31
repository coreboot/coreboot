/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Digital Design Corporation
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

#ifndef SUPERIO_SMSC_LPC47B272_H
#define SUPERIO_SMSC_LPC47B272_H

#define LPC47B272_FDC              0   /* Floppy */
#define LPC47B272_PP               3   /* Parallel Port */
#define LPC47B272_SP1              4   /* Com1 */
#define LPC47B272_SP2              5   /* Com2 */
#define LPC47B272_KBC              7   /* Keyboard & Mouse */
#define LPC47B272_RT              10   /* Runtime reg*/

#define LPC47B272_MAX_CONFIG_REGISTER	0x5F

#include <arch/io.h>
#include <stdint.h>

void lpc47b272_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif /* SUPERIO_SMSC_LPC47B272_H */
