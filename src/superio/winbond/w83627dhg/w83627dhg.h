/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Uwe Hermann <uwe@hermann-uwe.de>
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

#ifndef SUPERIO_WINBOND_W83627DHG_W83627DHG_H
#define SUPERIO_WINBOND_W83627DHG_W83627DHG_H

#define W83627DHG_FDC              0   /* Floppy */
#define W83627DHG_PP               1   /* Parallel port */
#define W83627DHG_SP1              2   /* Com1 */
#define W83627DHG_SP2              3   /* Com2 */
#define W83627DHG_KBC              5   /* PS/2 keyboard + mouse */
#define W83627DHG_SPI              6   /* Serial peripheral interface */
#define W83627DHG_GPIO6            7   /* GPIO6 */
#define W83627DHG_WDTO_PLED        8   /* WDTO#, PLED */
#define W83627DHG_GPIO2345         9   /* GPIO2, GPIO3, GPIO4, GPIO5 */
#define W83627DHG_ACPI            10   /* ACPI */
#define W83627DHG_HWM             11   /* Hardware monitor */
#define W83627DHG_PECI_SST        12   /* PECI, SST */

#endif
