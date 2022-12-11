/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/google/jecht/onboard.h>

/*
 * LAN connected to Root Port 3, becomes Root Port 1 after coalesce
 */
Scope (\_SB.PCI0.RP01)
{
	Device (ETH0)
	{
		Name (_ADR, 0x00000000)
		Name (_PRW, Package() { JECHT_NIC_WAKE_GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (JECHT_NIC_WAKE_GPIO, Local0)

			If (Arg0 == 1) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
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
		Name (_PRW, Package() { JECHT_WLAN_WAKE_GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			Store (JECHT_WLAN_WAKE_GPIO, Local0)

			If (Arg0 == 1) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}
	}
}

/* USB port entries */
#include <variant/acpi/usb.asl>
