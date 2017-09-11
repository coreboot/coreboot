/* SPDX-License-Identifier: GPL-2.0-only */

/* mainboard configuration */
#include "../ec.h"

#define SIO_EC_ENABLE_PS2K       // Enable PS/2 Keyboard

/* SIO_EC_ENABLE_PS2M is not defined in favor of custom ASL below */

/* ACPI code for EC SuperIO functions */
#include <ec/quanta/it8518/acpi/superio.asl>

/* Use multitouch trackpad */
Scope (\_SB.PCI0.LPCB.SIO)
{
	Device (PS2M)
	{
		Name (_HID, EISAID("LEN2011"))
		Name (_CID, EISAID("PNP0F13"))

		Method (_STA, 0, NotSerialized)
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate()
		{
			FixedIO (0x60, 0x01)
			FixedIO (0x64, 0x01)
			IRQNoFlags () {12}
		})
	}
}
