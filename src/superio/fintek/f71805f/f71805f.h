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

#ifndef SUPERIO_FINTEK_F71805F_F71805F_H
#define SUPERIO_FINTEK_F71805F_F71805F_H

/*
 * Datasheet:
 *  - Name: F71805F/FG Super H/W Monitor + LPC IO
 *  - URL: http://www.fintek.com.tw/eng/products.asp?BID=1&SID=17
 *  - PDF: http://www.fintek.com.tw/files/productfiles/F71805F_V025.pdf
 *  - Revision: V0.25P
 */

/* Logical Device Numbers (LDN). */
#define F71805F_FDC	0x00	/* Floppy */
#define F71805F_SP1	0x01	/* UART1 */
#define	F71805F_SP2	0x02	/* UART2 */
#define F71805F_PP	0x03	/* Parallel port */
#define	F71805F_HWM	0x04	/* Hardware monitor */
#define	F71805F_GPIO	0x06	/* General Purpose I/O (GPIO) */
#define	F71805F_PME	0x0a	/* Power Management Events (PME) */

#endif
