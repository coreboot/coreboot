/* SPDX-License-Identifier: GPL-2.0-only */

/* Dock Super I/O devices. */
Device (DSIO)
{
	Name (_HID, EISAID("PNP0A05"))
	Name (_UID, 0x02)

	Method (_STA, 0, NotSerialized)
	{
		If (\_SB.DOCK._STA ()) {
			Return (0x0f)
		}

		Return (0x00)
	}

	Device (DURT)
	{
		Name (_HID, EISAID("PNP0501"))
		Name (_UID, 0x01)
		Name (_EJD, "_SB.DOCK")

		Method (_STA, 0, NotSerialized)
		{
			If (\_SB.DOCK._STA ()) {
				Return (0x0f)
			}

			Return (0x00)
		}

		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x03f8, 0x03f8, 0x08, 0x08)
			IRQNoFlags () { 4 }
		})
	}

	Device (DLPT)
	{
		Name (_HID, EISAID("PNP0400"))
		Name (_UID, 0x01)
		Name (_EJD, "_SB.DOCK")

		Method (_STA, 0, NotSerialized)
		{
			If (\_SB.DOCK._STA ()) {
				Return (0x0f)
			}

			Return (0x00)
		}

		Name (_CRS, ResourceTemplate () {
			IO (Decode16, 0x03bc, 0x03bc, 0x01, 0x03)
			IRQNoFlags () { 7 }
		})
	}
}
