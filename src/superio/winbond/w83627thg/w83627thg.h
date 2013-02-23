/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 * Copyright (C) 2003-2004 Linux Networx
 * Copyright (C) 2004 Tyan
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

#ifndef SUPERIO_WINBOND_W83627THG_W83627THG_H
#define SUPERIO_WINBOND_W83627THG_W83627THG_H

#define W83627THG_FDC              0   /* Floppy */
#define W83627THG_PP               1   /* Parallel port */
#define W83627THG_SP1              2   /* Com1 */
#define W83627THG_SP2              3   /* Com2 */
#define W83627THG_KBC              5   /* PS/2 keyboard & mouse */
#define W83627THG_GAME_MIDI_GPIO1  7
#define W83627THG_GPIO2            8
#define W83627THG_GPIO3            9
#define W83627THG_ACPI            10
#define W83627THG_HWM             11   /* Hardware monitor */

void w83627thg_set_clksel_48(device_t dev);

#endif
