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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef SUPERIO_NUVOTON_NCT5104D_NCT5104D_H
#define SUPERIO_NUVOTON_NCT5104D_NCT5104D_H

/* Logical Device Numbers (LDN). */
#define NCT5104D_SP1  0x02 /* Com1 */
#define NCT5104D_SP2  0x03 /* Com2 */
#define NCT5104D_GPIO_WDT 0x08 /* GPIO WDT Interface */
#define NCT5104D_GPIO_PP_OD 0xF /* GPIO Push-Pull / Open drain select  */

/* Virtual Logical Device Numbers (LDN) */
#define NCT5104D_GPIO_V 0x07 /* GPIO - 0,1,6 Interface */

/* Virtual devices sharing the enables are encoded as follows:
	VLDN = baseLDN[7:0] | [10:8] bitpos of enable in 0x30 of baseLDN
*/
#define NCT5104D_GPIO0 ((0 << 8) | NCT5104D_GPIO_V)
#define NCT5104D_GPIO1 ((1 << 8) | NCT5104D_GPIO_V)
#define NCT5104D_GPIO6 ((6 << 8) | NCT5104D_GPIO_V)

#endif
