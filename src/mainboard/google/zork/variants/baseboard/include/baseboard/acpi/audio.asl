/* SPDX-License-Identifier: GPL-2.0-or-later */

Scope (EC0.CREC) {
	#include <ec/google/chromeec/acpi/codec.asl>
}

/* machine driver */
Device (I2SM)
{
	Name (_HID, "AMDI5682")
	Name (_UID, 1)
	Name (_DDN, "I2S machine Driver")

	Name (_CRS, ResourceTemplate ()
	{
#if CONFIG(BOARD_GOOGLE_BASEBOARD_DALBOZ)
		/* DMIC select GPIO */
		GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
			IoRestrictionNone, "\\_SB.GPIO", 0x00,
			ResourceConsumer,,) { 6 }
#else
		/* DMIC select GPIO */
		GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
			IoRestrictionNone, "\\_SB.GPIO", 0x00,
			ResourceConsumer,,) { 13 }
#endif
	})
	/* Device-Specific Data */
	Name (_DSD, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package ()
			{
				"dmic-gpio", Package () { ^I2SM, 0, 0, 0 }
			}
		}

	})
	Method (_STA)
	{
		Return (0xF)
	}
}
