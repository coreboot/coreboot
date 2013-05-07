/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Sean Nelson <snelson@nmt.edu>
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

#ifndef SUPERIO_WINBOND_W83697HF_W83697HF_H
#define SUPERIO_WINBOND_W83697HF_W83697HF_H

#define W83697HF_FDC            0    /* Floppy */
#define W83697HF_PP             1    /* Parallel port */
#define W83697HF_SP1            2    /* Com1 */
#define W83697HF_SP2            3    /* Com2 */
#define W83697HF_CIR            6    /* Consumer IR */
#define W83697HF_GAME_GPIO1     7    /* Game port, GPIO 1 */
#define W83697HF_MIDI_GPIO5     8    /* MIDI, GPIO 5 */
#define W83697HF_GPIO234        9    /* GPIO 2, 3, 4 */
#define W83697HF_ACPI           10   /* ACPI */
#define W83697HF_HWM            11   /* Hardware monitor */

void w83697hf_set_clksel_48(device_t);
#endif
