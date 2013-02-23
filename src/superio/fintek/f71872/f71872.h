/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Corey Osgood <corey@slightlyhackish.com>
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

#ifndef SUPERIO_FINTEK_F71872_F71872_H
#define SUPERIO_FINTEK_F71872_F71872_H

/* Logical Device Numbers (LDN). */
#define F71872_FDC	0x00	/* Floppy */
#define F71872_SP1	0x01	/* UART1 */
#define F71872_SP2	0x02	/* UART2 */
#define F71872_PP	0x03	/* Parallel Port */
#define F71872_HWM	0x04	/* Hardware Monitor */
#define F71872_KBC	0x05	/* Keyboard/Mouse */
#define F71872_GPIO	0x06	/* GPIO */
#define F71872_VID	0x07	/* VID */
#define F71872_PM	0x0a	/* ACPI/PME */

#endif
