/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/google/butterfly/onboard.h>

Scope (\_SB) {
	Device (LID0)
	{
		Name (_HID, EisaId("PNP0C0D"))
		Method (_LID, 0)
		{
			\LIDS = \_SB.PCI0.LPCB.EC0.LIDF
			Return (\LIDS)
		}
	}

	Device (TPAD)
	{
		Name (_UID, 1)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name(_HID, EisaId("PNP0C0E"))

		// Trackpad Wake is GPIO11, wake from S3
		Name(_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x03 })

		Name(_CRS, ResourceTemplate()
		{
			// PIRQG -> GSI22
			Interrupt (ResourceConsumer, EDGE, ActiveLow)
			{
				BOARD_TRACKPAD_IRQ
			}

			// SMBUS Address 0x67
			VendorShort (ADDR) { BOARD_TRACKPAD_I2C_ADDR }
		})
	}

}

// Battery information
Name (BATV, "GOOGLE")
