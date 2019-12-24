/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2020 Intel Corp.
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

Device (PEPD)
{
	Name (_HID, "INT33A1" /* Intel Power Engine */)
	Name (_CID, EisaId ("PNP0D80") /* System Power Management Controller */)
	Name (_UID, One)
}
