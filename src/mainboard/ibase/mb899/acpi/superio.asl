/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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


Device (SIO1)
{
	Name (_HID, EISAID("PNP0A05"))
	Name (_UID, 1)

	Device (UAR1)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 1)
		Name(_DDN, "COM1")

		Method (_STA, 0)
		{
			// always enable for now
			Return (0x0f)
		}

		Method (_DIS, 0) { /* NOOP */ }

		Name (_PRS, ResourceTemplate() {
			StartDependentFn(0, 1) {
				IO(Decode16, 0x3f8, 0x3f8, 0x8, 0x8)
				IRQNoFlags() { 4 }
			} EndDependentFn()
		})

		Method (_CRS, 0)
		{
			Return(ResourceTemplate() {
				IO(Decode16, 0x3f8, 0x3f8, 0x8, 0x8)
				IRQNoFlags() { 4 }
			})
		}
		// Some methods need an implementation here:
		// missing: _STA, _DIS, _CRS, _PRS,
		// missing: _SRS, _PS0, _PS3
	}

	Device (UAR2)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 2)
		Name(_DDN, "COM2")

		Method (_STA, 0)
		{
			// always enable for now
			Return (0x0f)
		}

		Method (_DIS, 0) { /* NOOP */ }

		Name (_PRS, ResourceTemplate() {
			StartDependentFn(0, 1) {
				IO(Decode16, 0x2f8, 0x2f8, 0x8, 0x8)
				IRQNoFlags() { 3 }
			} EndDependentFn()
		})

		Method (_CRS, 0)
		{
			Return(ResourceTemplate() {
				IO(Decode16, 0x2f8, 0x2f8, 0x8, 0x8)
				IRQNoFlags() { 3 }
			})
		}
		// Some methods need an implementation here:
		// missing: _STA, _DIS, _CRS, _PRS,
		// missing: _SRS, _PS0, _PS3
	}
}

