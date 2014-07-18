/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Felix Held <felix-coreboot@felixheld.de>
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

#ifndef SUPERIO_NUVOTON_NCT5572D
#define SUPERIO_NUVOTON_NCT5572D

/* Logical Device Numbers (LDN). */
#define NCT5572D_SP1		0x02 /* Com1 */
#define NCT5572D_IR		0x03
#define NCT5572D_KBC		0x05
#define NCT5572D_CIR		0x06
#define NCT5572D_GPIO689_V	0x07
#define NCT5572D_WDT1		0x08
#define NCT5572D_GPIO235_V	0x09
#define NCT5572D_ACPI		0x0A
#define NCT5572D_HWM_TSI_FPLED	0x0B /* HW monitor/SB-TSI/front LED/deep S5 */
#define NCT5572D_PECI		0x0C
#define NCT5572D_SUSLED		0x0D /* marked as reserved, but described */
#define NCT5572D_CIRWKUP	0x0E /* CIR wakeup */
#define NCT5572D_GPIO_PP_OD	0x0F /* GPIO Push-Pull/Open drain select */

/* virtual LDN for GPIO */
#define NCT5572D_GPIO2		((0 << 8) | NCT5572D_GPIO235_V)
#define NCT5572D_GPIO3		((1 << 8) | NCT5572D_GPIO235_V)
#define NCT5572D_GPIO5		((3 << 8) | NCT5572D_GPIO235_V)
#define NCT5572D_GPIO6		((1 << 8) | NCT5572D_GPIO689_V)
#define NCT5572D_GPIO8		((3 << 8) | NCT5572D_GPIO689_V)
#define NCT5572D_GPIO9		((4 << 8) | NCT5572D_GPIO689_V)

#endif /* SUPERIO_NUVOTON_NCT5572D */
