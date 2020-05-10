/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/google/link/onboard.h>

Scope (\_SB) {
	Device (TPAD)
	{
		Name (_UID, 1)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name(_HID, EisaId("PNP0C0E"))

		// Trackpad Wake is GPIO12
		Name(_PRW, Package() { BOARD_TRACKPAD_WAKE_GPIO, 0x03 } )

		Name(_CRS, ResourceTemplate()
		{
			// PIRQE -> GSI20
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				BOARD_TRACKPAD_IRQ
			}

			// SMBUS Address 0x4b
			VendorShort (ADDR) { BOARD_TRACKPAD_I2C_ADDR }
		})
	}

	Device (TSCR)
	{
		Name (_UID, 2)

		// Report as a Sleep Button device so Linux will
		// automatically enable it as a wake source
		Name(_HID, EisaId("PNP0C0E"))

		// Touchscreen Wake is GPIO14
		Name(_PRW, Package(){0x1e, 0x03})

		Name(_CRS, ResourceTemplate()
		{
			// PIRQG -> GSI22
			Interrupt (ResourceConsumer, Edge, ActiveLow) {22}

			// SMBUS Address 0x4a
			VendorShort (ADDR) {0x4a}
		})
	}
}
