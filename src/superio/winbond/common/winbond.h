/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
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

#ifndef SUPERIO_WINBOND_COMMON_ROMSTAGE_H
#define SUPERIO_WINBOND_COMMON_ROMSTAGE_H

#include <arch/io.h>
#include <stdint.h>

void winbond_enable_serial(pnp_devfn_t dev, u16 iobase);

#endif /* SUPERIO_WINBOND_COMMON_ROMSTAGE_H */
