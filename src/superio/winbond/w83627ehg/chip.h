/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#ifndef SIO_COM1
#define SIO_COM1_BASE 0x3f8
#endif
#ifndef SIO_COM2
#define SIO_COM2_BASE 0x2f8
#endif

extern struct chip_operations superio_winbond_w83627ehg_ops;

#include <pc80/keyboard.h>
#include <uart8250.h>

struct superio_winbond_w83627ehg_config {
	struct uart8250 com1, com2;
	struct pc_keyboard keyboard;
};
