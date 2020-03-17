/*
 * This file is part of the coreboot project.
 *
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

#include <southbridge/intel/bd82x6x/pch.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, 0 },
	{ 1, 1, 1 },
	{ 1, 1, 3 },
	{ 1, 1, 3 },
	{ 1, 1, -1 },
	{ 1, 1, -1 },
	{ 1, 0, 2 },
	{ 1, 0, 2 },
	{ 1, 1, 6 },
	{ 1, 1, 5 },
	{ 1, 1, 6 },
	{ 1, 1, 6 },
	{ 1, 1, 7 },
	{ 1, 1, 6 },
};
