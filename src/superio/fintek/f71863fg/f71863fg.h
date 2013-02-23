/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Wang Qing Pei <wangqingpei@gmail.com>
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

#ifndef SUPERIO_FINTEK_F71863FG_F71863FG_H
#define SUPERIO_FINTEK_F71863FG_F71863FG_H

/* Logical Device Numbers (LDN). */
#define F71863FG_FDC	0x00	/* Floppy */
#define F71863FG_SP1	0x01	/* UART1 */
#define	F71863FG_SP2	0x02	/* UART2 */
#define F71863FG_PP	0x03	/* Parallel port */
#define	F71863FG_HWM	0x04	/* Hardware monitor */
#define	F71863FG_KBC	0x05	/* PS/2 keyboard and mouse */
#define	F71863FG_GPIO	0x06	/* General Purpose I/O (GPIO) */
#define	F71863FG_VID	0x07	/* VID */
#define	F71863FG_SPI	0x08	/* SPI */
#define	F71863FG_PME	0x0a	/* Power Management Events (PME) and ACPI */

#endif
