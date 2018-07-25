/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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
 */

#include <onboard.h>

/*
 * LAN connected to Root Port 3, becomes Root Port 1 after coalesce
 */
Scope (\_SB.PCI0.RP01)
{
	Device (ETH0)
	{
		Name (_ADR, 0x00000000)
		Name (_PRW, Package() { NIC_WAKE_GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (NIC_WAKE_GPIO, Local0)

			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GWAK (Local0)
			}
		}
	}
}

/*
 * WLAN connected to Root Port 4, becomes Root Port 2 after coalesce
 */
Scope (\_SB.PCI0.RP02)
{
	Device (WLAN)
	{
		Name (_ADR, 0x00000000)
		Name (_PRW, Package() { WLAN_WAKE_GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (WLAN_WAKE_GPIO, Local0)

			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GWAK (Local0)
			}
		}
	}
}

/* USB port entries */
#include "acpi/usb.asl"
