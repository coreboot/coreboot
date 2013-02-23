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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * Datasheet:
 *  - Name: F81865F/F-I
 */
#ifndef SUPERIO_FINTEK_F81865_F81865_H
#define SUPERIO_FINTEK_F81865_F81865_H

/* Logical Device Numbers (LDN). */
#define	F81865F_FDC	0x00	/* Floppy */
#define	F81865F_SP1	0x10	/* UART1 */
#define	F81865F_SP2	0x11	/* UART2 */
#define	F81865F_PP	0x03	/* Parallel Port */
#define	F81865F_HWM	0x04	/* Hardware Monitor */
#define	F81865F_KBC	0x05	/* Keyboard/Mouse */
#define	F81865F_GPIO	0x06	/* General Purpose I/O (GPIO) */
#define	F81865F_PME	0x0a	/* Power Management Events (PME) */

#endif
