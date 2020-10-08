/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/samsung/lumpy/onboard.h>

Scope (\_SB) {
	Device (LID0)
	{
		Name(_HID, EisaId("PNP0C0D"))
		Method(_LID, 0)
		{
			\LIDS = \_SB.PCI0.LPCB.EC0.LIDS
			Return (\LIDS)
		}

		// WAKE SCI# from EC is GPIO14
		Name(_PRW, Package(){0x1e, 0x05})
	}

	Device (PWRB)
	{
		Name(_HID, EisaId("PNP0C0C"))
	}

	Device (TPAD)
	{
		// Report as a Sleep Button device so
		// Linux will automatically enable for wake
		Name(_HID, EisaId("PNP0C0E"))

		// Trackpad Wake is GPIO11
		Name(_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x03 })

		Name(_CRS, ResourceTemplate()
		{
			// PIRQF -> GSI21
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TRACKPAD_IRQ
			}

			// SMBUS Address 0x67
			VendorShort (ADDR) { BOARD_TRACKPAD_I2C_ADDR }
		})
	}

	Device (LITE)
	{
		// Generic ACPI Device Container
		Name(_HID, EisaId("PNP0A05"))

		Name(_CRS, ResourceTemplate()
		{
			// PIRQE -> GSI20
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_LIGHTSENSOR_IRQ
			}

			// SMBUS Address 0x44
			VendorShort (ADDR) { BOARD_LIGHTSENSOR_I2C_ADDR }
		})
	}
}

// Battery information
Name (BATV, "SAMSUNG")
Name (BATM, "LUMPY")
Name (BATS, "BATTERY")

/* USB port entries */
#include "acpi/usb.asl"
