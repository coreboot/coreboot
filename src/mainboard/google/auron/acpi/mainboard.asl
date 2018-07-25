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
 */

#include <variant/onboard.h>

Scope (\_SB.PCI0.RP01)
{
	Device (WLAN)
	{
		Name (_ADR, 0x00000000)

		Name (_PRW, Package() { BOARD_WLAN_WAKE_GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (BOARD_WLAN_WAKE_GPIO, Local0)
			If (LEqual (Arg0, 1)) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}
	}
}

#include <variant/acpi/mainboard.asl>

/* USB port entries */
#include <variant/acpi/usb.asl>
