/*
 * superio.c: RAM driver for SMSC LPC47M10X2 Super I/O chip
 *
 * Copyright 2000  AG Electronics Ltd.
 * Copyright 2003-2004 Linux Networx
 * Copyright 2004 Tyan
 * Copyright (C) 2005 Digital Design Corporation
 * Copyright (C) Ron Minnich, LANL
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#define LPC47M10X2_FDC              0   /* Floppy */
#define LPC47M10X2_PP               3   /* Parallel Port */
#define LPC47M10X2_SP1              4   /* Com1 */
#define LPC47M10X2_SP2              5   /* Com2 */
#define LPC47M10X2_KBC              7   /* Keyboard & Mouse */
#define LPC47M10X2_GAME             9  /* GAME */
#define LPC47M10X2_PME             10   /* PME  reg*/
#define LPC47M10X2_MPU           10   /* MPE -- who knows --   reg*/

#define LPC47M10X2_MAX_CONFIG_REGISTER	0x5F
