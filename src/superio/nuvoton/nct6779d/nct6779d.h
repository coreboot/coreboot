/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Alec Ari <neotheuser@ymail.com>
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

/* Logical Device Numbers (LDN). */
#define NCT6779D_ACPI 0x0a /* ACPI */

/* Should NCT6779D_HWMN be a virtual LDN since it is for Hardware Monitor and Front Panel LED? */
#define NCT6779D_HWMN 0x0b /* Hw-mon / Front Pan LED */

#define NCT6779D_WDT1 0x0d /* WDT1 */
#define NCT6779D_CIRW 0x0e /* CIR Wake-up */
#define NCT6779D_PPOD 0x0f /* GPIO Push-Pull / Open drain select  */
#define NCT6779D_PAR  0x01 /* Parallel Port */
#define NCT6779D_SP1  0x02 /* Com1 */

/* Should NCT6779D_SP2 be a virtual LDN since it is for UART B & IR? */
#define NCT6779D_SP2  0x03 /* Com2 (UART B & IR) */

#define NCT6779D_KBD  0x05 /* Keyboard Controller */
#define NCT6779D_CIR  0x06 /* CIR */

/* Should NCT6779D_GPIO_WDT be a virtual LDN since it is for WDT1, GPIO 0,1? */
#define NCT6779D_GPIO_WDT 0x08 /* GPIO WDT Interface */

#define NCT6779D_UART 0x14 /* Port 80 UART */
#define NCT6779D_DSLP 0x16 /* Deep Sleep */

/* Virtual Logical Device Numbers (LDN) */
/* What is going on here? */
#define NCT6779D_GPIO_ALL_V 0x09 /* GPIO - 1,2,3,4,5,6,7,8  */

/* Why does this LDN share some of the same functions from 0x09? */
#define NCT6779D_GPIO_V 0x07 /* GPIO - 6,7,8 */

/* Virtual devices sharing the enables are encoded as follows:
	VLDN = baseLDN[7:0] | [10:8] bitpos of enable in 0x30 of baseLDN
*/

/* No idea what I'm doing from this point on */
#define NCT6779D_GPIO6 ((6 << 8) | NCT6779D_GPIO_V)
#define NCT6779D_GPIO7 ((7 << 8) | NCT6779D_GPIO_V)
#define NCT6779D_GPIO8 ((8 << 8) | NCT6779D_GPIO_V)

/* NCT6779D_GPIO_ALL_V overlaps NCT6779D_GPIO_V (6,7,8 are overlapped) */
#define NCT6779D_GPIO1_ALL ((1 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO2_ALL ((2 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO3_ALL ((3 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO4_ALL ((4 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO5_ALL ((5 << 8) | NCT6779D_GPIO_ALL_V)

/* GPIO6,7,8_ALL need different values than GPIO6,7,8 to avoid being redefined */
#define NCT6779D_GPIO6_ALL ((9 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO7_ALL ((10 << 8) | NCT6779D_GPIO_ALL_V)
#define NCT6779D_GPIO8_ALL ((11 << 8) | NCT6779D_GPIO_ALL_V)

#endif
