/* SPDX-License-Identifier: GPL-2.0-only */

Device (PS2M)		// PS/2 Mouse
{
	Name (_HID, EISAID (CONFIG_PS2M_EISAID))
	Name (_CID, EISAID ("PNP0F13"))

	Name (_CRS, ResourceTemplate()
	{
		IRQ (Edge, ActiveHigh, Exclusive) { 0x0C } // IRQ 12
	})

	Method (_STA, 0)
	{
		Return (0x0F)
	}
}
