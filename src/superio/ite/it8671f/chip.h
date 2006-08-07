/*
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef _SUPERIO_ITE_IT8671F
#define _SUPERIO_ITE_IT8671F

// TODO: Unused?
// #ifndef SIO_COM1
// #define SIO_COM1_BASE   0x3F8
// #endif
// #ifndef SIO_COM2
// #define SIO_COM2_BASE   0x2F8
// #endif

#include <pc80/keyboard.h>
#include <uart8250.h>

extern struct chip_operations superio_ITE_it8671f_ops;

struct superio_ITE_it8671f_config {
	struct uart8250 com1, com2;
	struct pc_keyboard keyboard;
};

#endif /* _SUPERIO_ITE_IT8671F */

