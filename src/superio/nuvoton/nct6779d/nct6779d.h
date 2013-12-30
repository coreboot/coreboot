/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Alec Ari <neotheuser@ymail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef SUPERIO_NUVOTON_NCT6779D_NCT6779D_H
#define SUPERIO_NUVOTON_NCT6779D_NCT6779D_H

#define NCT6779D_PAR  0x01 /* Parallel Port */
#define NCT6779D_SP1  0x02 /* Com1 */
#define NCT6779D_SP2  0x03 /* Com2 (UART B & IR) */
#define NCT6779D_KBC  0x05 /* Keyboard Controller */
#define NCT6779D_CIR  0x06 /* CIR */
#define NCT6779D_HWMN 0x0b /* Hw-mon / Front panel LED */

#define NCT6779D_WDT_GPIO_V 0x08
#define NCT6779D_GPIO_ALL_V 0x09

#define NCT6779D_WDT1	((0 << 8) | NCT6779D_WDT_GPIO_V)
#define NCT6779D_GPIO0	((1 << 8) | NCT6779D_WDT_GPIO_V)

#define NCT6779D_GPIO1	((1 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO2	((2 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO3	((3 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO4	((4 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO5	((5 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO6	((6 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO7	((7 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO8	((0 << 8) | NCT6779D_GPIO_ALL_V)

#endif
