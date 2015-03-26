/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

Device (XHCI)
{
	Name (_ADR, 0x00140000)
	Name (_PRW, Package () { 0x0d, 3 })
	Name (_S3D, 3) /* Highest D state in S3 state */

	Device (RHUB)
	{
		Name (_ADR, 0x00000000)
		Device (PRT1) { Name (_ADR, 1) }
		Device (PRT2) { Name (_ADR, 2) }
		Device (PRT3) { Name (_ADR, 3) }
		Device (PRT4) { Name (_ADR, 4) }
	}
}
