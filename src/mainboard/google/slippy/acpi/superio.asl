/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

/* mainboard configuration */
#include <mainboard/google/slippy/ec.h>

#define SIO_EC_MEMMAP_ENABLE     // EC Memory Map Resources
#define SIO_EC_HOST_ENABLE       // EC Host Interface Resources
#define SIO_EC_ENABLE_PS2K       // Enable PS/2 Keyboard
#define SIO_EC_ENABLE_COM1       // Enable Serial Port 1

/* ACPI code for EC SuperIO functions */
#include <ec/google/chromeec/acpi/superio.asl>
