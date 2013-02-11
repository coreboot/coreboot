/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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

// Scope is \_SB.PCI0.LPCB

Device (SIO) {
	Name (_UID, 0)
	Name (_ADR, 0)


#ifdef SIO_EC_ENABLE_PS2K
	Device (PS2K)		// Keyboard
	{
		Name (_UID, 0)
		Name (_ADR, 0)
		Name (_HID, EISAID("PNP0303"))
		Name (_CID, EISAID("PNP030B"))

		Method (_STA, 0, NotSerialized) {
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate()
		{
			FixedIO (0x60, 0x01)
			FixedIO (0x64, 0x01)
			IRQNoFlags () {1}
		})

		Name (_PRS, ResourceTemplate()
		{
			StartDependentFn (0, 0) {
				FixedIO (0x60, 0x01)
				FixedIO (0x64, 0x01)
				IRQNoFlags () {1}
			}
			EndDependentFn ()
		})
	}
#endif
}
