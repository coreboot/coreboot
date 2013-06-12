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

/*
 * Chrome OS Embedded Controller interface
 *
 * Constants that should be defined:
 *
 * SIO_EC_MEMMAP_ENABLE     : Enable EC LPC memory map resources
 * EC_LPC_ADDR_MEMMAP       : Base address of memory map range
 * EC_MEMMAP_SIZE           : Size of memory map range
 *
 * SIO_EC_HOST_ENABLE       : Enable EC host command interface resources
 * EC_LPC_ADDR_HOST_DATA    : EC host command interface data port
 * EC_LPC_ADDR_HOST_CMD     : EC host command interface command port
 * EC_HOST_CMD_REGION0      : EC host command buffer
 * EC_HOST_CMD_REGION1      : EC host command buffer
 * EC_HOST_CMD_REGION_SIZE  : EC host command buffer size
 */

// Scope is \_SB.PCI0.LPCB

Device (SIO) {
	Name (_UID, 0)
	Name (_ADR, 0)

#ifdef SIO_EC_MEMMAP_ENABLE
	Device (ECMM) {
		Name (_HID, EISAID ("PNP0C02"))
		Name (_UID, 1)
		Name (_ADR, 0)

		Method (_STA, 0, NotSerialized) {
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			FixedIO (EC_LPC_ADDR_MEMMAP, EC_MEMMAP_SIZE)
		})

		Name (_PRS, ResourceTemplate ()
		{
			FixedIO (EC_LPC_ADDR_MEMMAP, EC_MEMMAP_SIZE)
		})
	}
#endif

#ifdef SIO_EC_HOST_ENABLE
	Device (ECUI) {
		Name (_HID, EISAID ("PNP0C02"))
		Name (_UID, 3)
		Name (_ADR, 0)

		Method (_STA, 0, NotSerialized) {
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			FixedIO (EC_LPC_ADDR_HOST_DATA, 1)
			FixedIO (EC_LPC_ADDR_HOST_CMD, 1)
			FixedIO (EC_HOST_CMD_REGION0, EC_HOST_CMD_REGION_SIZE)
			FixedIO (EC_HOST_CMD_REGION1, EC_HOST_CMD_REGION_SIZE)
		})

		Name (_PRS, ResourceTemplate ()
		{
			StartDependentFn (0, 0) {
				FixedIO (EC_LPC_ADDR_HOST_DATA, 1)
				FixedIO (EC_LPC_ADDR_HOST_CMD, 1)
				FixedIO (EC_HOST_CMD_REGION0,
					EC_HOST_CMD_REGION_SIZE)
				FixedIO (EC_HOST_CMD_REGION1,
					EC_HOST_CMD_REGION_SIZE)
			}
			EndDependentFn ()
		})
	}
#endif

#ifdef SIO_EC_ENABLE_COM1
	Device (COM1) {
		Name (_HID, EISAID ("PNP0501"))
		Name (_UID, 1)
		Name (_ADR, 0)

		Method (_STA, 0, NotSerialized) {
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			FixedIO (0x03F8, 0x08)
			IRQNoFlags () {4}
		})

		Name (_PRS, ResourceTemplate ()
		{
			StartDependentFn (0, 0) {
				FixedIO (0x03F8, 0x08)
				IRQNoFlags () {4}
			}
			EndDependentFn ()
		})
	}
#endif

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
