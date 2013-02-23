/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Dynon Avionics
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 $
 */

#ifndef SUPERIO_WINBOND_W83627UHG_W83627UHG_H
#define SUPERIO_WINBOND_W83627UHG_W83627UHG_H

#define W83627UHG_FDC			0   /* Floppy */
#define W83627UHG_PP			1   /* Parallel port */
#define W83627UHG_SP1			2   /* Com1 */
#define W83627UHG_SP2			3   /* Com2 */
#define W83627UHG_KBC			5   /* PS/2 keyboard & mouse */
#define W83627UHG_SP3			6   /* Com3 */
#define W83627UHG_GPIO3_4		7   /* GPIO 3/4 */
#define W83627UHG_WDTO_PLED_GPIO5_6	8   /* WDTO#, PLED, GPIO5/6 */
#define W83627UHG_GPIO1_2		9   /* GPIO 1/2, SUSLED */
#define W83627UHG_ACPI			10  /* ACPI */
#define W83627UHG_HWM			11  /* Hardware monitor */
#define W83627UHG_PECI_SST		12  /* PECI, SST */
#define W83627UHG_SP4			13  /* Com4 */
#define W83627UHG_SP5			14  /* Com5 */
#define W83627UHG_SP6			15  /* Com6 */

#endif
