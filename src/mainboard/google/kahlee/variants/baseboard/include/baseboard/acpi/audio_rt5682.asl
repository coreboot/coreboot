/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio_map.h>

/* Grunt specific I2S machine driver */
Device (I2S1)
{
	Name (_HID, "AMDI5682")
	Name (_CID, "AMDI5682")

	/* Device-Specific Data */
	Name (_DSD, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package () { "bt-pad-enable", 1 },
		}
	})

	Method (_CRS, 0x0, Serialized) {
		Name (RBUF, ResourceTemplate () {
			// Memory resource is for MISC FCH register set.
			// It is needed for enabling the clock.
			Memory32Fixed(ReadWrite, ACPIMMIO_MISC_BASE, 0x100)
		})

		Return (RBUF)
	}
}
