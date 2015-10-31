/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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

#ifndef SUPERIO_NUVOTON_WPCM450_WPCM450_H
#define SUPERIO_NUVOTON_WPCM450_WPCM450_H

#define WPCM450_SP2  0x02 /* Com2 */
#define WPCM450_SP1  0x03 /* Com1 */
#define WPCM450_KBCK 0x06 /* Keyboard */

void wpcm450_enable_dev(u8 dev, u16 port, u16 iobase);

#endif
