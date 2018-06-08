/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define IT8516E_EC_DEV		EC0
#define SUPERIO_PNP_BASE	0x20e
#define IT8516E_FIRST_DATA	0x62
#define IT8516E_FIRST_SC	0x66
#define IT8516E_SECOND_DATA	0x20c
#define IT8516E_SECOND_SC	0x20d
#include <ec/kontron/it8516e/acpi/ec.asl>
