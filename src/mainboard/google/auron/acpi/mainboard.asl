/* SPDX-License-Identifier: GPL-2.0-only */

#include <variant/onboard.h>

/*
 * WLAN on Buddy connected to RP4, becomes RP2 after coalesce
 */
#if CONFIG(BOARD_GOOGLE_BUDDY)
Scope (\_SB.PCI0.RP02)
#else
Scope (\_SB.PCI0.RP01)
#endif
{
	Device (WLAN)
	{
		Name (_ADR, 0x00000000)

		Name (_PRW, Package() { BOARD_WLAN_WAKE_GPIO, 3 })

		Method (_DSW, 3, NotSerialized)
		{
			Local0 = BOARD_WLAN_WAKE_GPIO
			If (Arg0 == 1) {
				// Enable GPIO as wake source
				\_SB.PCI0.LPCB.GPIO.GWAK (Local0)
			}
		}
	}
}

#include <variant/acpi/mainboard.asl>

/* USB port entries */
#include <variant/acpi/usb.asl>
