/* SPDX-License-Identifier: GPL-2.0-only */

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
			Local0 = NIC_WAKE_GPIO

			If (Arg0 == 1) {
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
			Local0 = WLAN_WAKE_GPIO

			If (Arg0 == 1) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GWAK (Local0)
			}
		}
	}
}

/* USB port entries */
#include "acpi/usb.asl"
