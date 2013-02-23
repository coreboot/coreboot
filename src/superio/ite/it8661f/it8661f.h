/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
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

#ifndef SUPERIO_ITE_IT8661F_IT8661F_H
#define SUPERIO_ITE_IT8661F_IT8661F_H

/* Datasheet: http://www.ite.com.tw/product_info/PC/Brief-IT8661_2.asp */

/* Logical device numbers (LDNs). */
#define IT8661F_FDC  0x00 /* Floppy */
#define IT8661F_SP1  0x01 /* Com1 */
#define IT8661F_SP2  0x02 /* Com2 */
#define IT8661F_PP   0x03 /* Parallel port */
#define IT8661F_IR   0x04 /* IR */
#define IT8661F_GPIO 0x05 /* GPIO & Alternate Function Configuration */

/* Register and bit definitions. */
#define IT8661F_REG_CC		0x02   /* Configure Control (write-only). */
#define IT8661F_REG_LDE		0x23   /* PnP Logical Device Enable. */
#define IT8661F_REG_SWSUSP	0x24   /* Software Suspend + Clock Select. */

#define IT8661F_ISA_PNP_PORT	0x0279 /* Write-only. */

#define IT8661F_CLKIN_24_MHZ	0
#define IT8661F_CLKIN_48_MHZ	1

/*
 * Special values used for entering MB PnP mode. The first four bytes of
 * each line determine the address port, the last four are data.
 */
static const u8 init_values[] = {
	0x6a, 0xb5, 0xda, 0xed, /**/ 0xf6, 0xfb, 0x7d, 0xbe,
	0xdf, 0x6f, 0x37, 0x1b, /**/ 0x0d, 0x86, 0xc3, 0x61,
	0xb0, 0x58, 0x2c, 0x16, /**/ 0x8b, 0x45, 0xa2, 0xd1,
	0xe8, 0x74, 0x3a, 0x9d, /**/ 0xce, 0xe7, 0x73, 0x39,
};

#endif
