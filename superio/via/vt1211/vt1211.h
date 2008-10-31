/*
 * This file is part of the coreboot project.
 *
 * Copyright 2007 Ronald G. Minnich <rminnich@gmail.com>
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

#ifndef SUPERIO_VIA_VT1211_VT122_H
#define SUPERIO_VIA_VT1211_VT122_H

#define VT1211_FDC              0   /* Floppy */
#define VT1211_PP               1   /* Parallel Port */
#define VT1211_SP1              2   /* Com1 */
#define VT1211_SP2              3   /* Com2 */
#define VT1211_KBC              5   /* Keyboard & Mouse */
#define VT1211_CIR              6
#define VT1211_GAME_MIDI_GPIO1  7
#define VT1211_GPIO2            8
#define VT1211_GPIO3            9
#define VT1211_ACPI            10
#define VT1211_HWM             11   /* Hardware Monitor */

#endif /* SUPERIO_VIA_VT1211_VT122_H */
