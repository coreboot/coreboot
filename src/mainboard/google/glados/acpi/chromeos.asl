/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
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
 * Foundation, Inc.
 */

#include "../gpio.h"

Name (OIPG, Package () {
	/* No physical recovery GPIO. */
	Package () { 0x0001, 0, 0xFFFFFFFF, "INT344B:00" },
	/* Firmware write protect GPIO. */
	Package () { 0x0003, 1, GPIO_PCH_WP, "INT344B:00" },
})
