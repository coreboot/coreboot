/*
 * This file is part of the LinuxBIOS project.

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
#ifndef SUPERIO_WINBOND_W83627HF_W83627HF_H
#define SUPERIO_WINBOND_W83627HF_W83627HF_H

#define W83627HF_FDC              0   /* Floppy */
#define W83627HF_PP               1   /* Parallel Port */
#define W83627HF_SP1              2   /* Com1 */
#define W83627HF_SP2              3   /* Com2 */
#define W83627HF_KBC              5   /* Keyboard & Mouse */
#define W83627HF_CIR              6
#define W83627HF_GAME_MIDI_GPIO1  7
#define W83627HF_GPIO2            8
#define W83627HF_GPIO3            9
#define W83627HF_ACPI            10
#define W83627HF_HWM             11   /* Hardware Monitor */
#endif /* SUPERIO_WINBOND_W83627HF_W83627HF_H */
