/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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


Device (SIO1)
{
	Name (_HID, EISAID("PNP0A05"))
	Name (_UID, 1)

	Device (UAR1)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 1)

		// Some methods need an implementation here:
		// missing: _STA, _DIS, _CRS, _PRS,
		// missing: _SRS, _PS0, _PS3
	}

	Device (UAR2)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 2)

		// Some methods need an implementation here:
		// missing: _STA, _DIS, _CRS, _PRS,
		// missing: _SRS, _PS0, _PS3
	}
}

