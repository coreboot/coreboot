/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/* Intel i82801G AC'97 Audio and Modem */

// Intel AC'97 Audio 0:1e.2

Device (AUD0)
{
	Name (_ADR, 0x001e0002)
}

// Intel AC'97 Modem 0:1e.3

Device (MODM)
{
	Name (_ADR, 0x001e0003)

	Name (_PRW, Package(){ 5, 4 })
}

