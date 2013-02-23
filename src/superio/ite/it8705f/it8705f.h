/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Uwe Hermann <uwe@hermann-uwe.de>
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

#ifndef SUPERIO_ITE_IT8705F_IT8705F_H
#define SUPERIO_ITE_IT8705F_IT8705F_H

/* Datasheet: http://www.ite.com.tw/product_info/PC/Brief-IT8705_2.asp */

/* Note: This should also work on an IT8705AF, they're almost the same. */

#define IT8705F_FDC  0x00 /* Floppy */
#define IT8705F_SP1  0x01 /* Com1 */
#define IT8705F_SP2  0x02 /* Com2 */
#define IT8705F_PP   0x03 /* Parallel port */
#define IT8705F_EC   0x04 /* Environment controller */
#define IT8705F_GPIO 0x05 /* GPIO */
#define IT8705F_GAME 0x06 /* GAME port */
#define IT8705F_IR   0x07 /* Consumer IR */
#define IT8705F_MIDI 0x08 /* MIDI port */

#endif
