/* SPDX-License-Identifier: GPL-2.0-only */

// Scope is \_SB.PCI0.LPCB

// Values, defined here, must match settings in devicetree.cb

Device (PS2M) {
	Name (_HID, EisaId ("PNP0F13"))
	Name (_CRS, ResourceTemplate () {
		IO (Decode16, 0x0060, 0x0060, 0x00, 0x01)
		IO (Decode16, 0x0064, 0x0064, 0x00, 0x01)
		IRQNoFlags () {12}
	})
	Method (_STA, 0, NotSerialized) {
		Local0 = FLG0 & 0x04
		If (Local0 == 0x04) {
			Return (0x0F)
		} Else {
			Return (0x00)
		}
	}
}

Device (PS2K) {
	Name (_HID, EisaId ("PNP0303"))
	Method (_STA, 0, NotSerialized) {
		Local0 = FLG0 & 0x04
		If (Local0 == 0x04) {
			Return (0x0F)
		} Else {
			Return (0x00)
		}
	}
	Name (_CRS, ResourceTemplate () {
		IO (Decode16, 0x0060, 0x0060, 0x00, 0x01)
		IO (Decode16, 0x0064, 0x0064, 0x00, 0x01)
		IRQNoFlags () {1}
	})
}

Device (COM1) {
	Name (_HID, EISAID ("PNP0501"))
	Name (_UID, 1)
	Method (_STA, 0, NotSerialized) {
		Local0 = FLG0 & 0x04
		If (Local0 == 0x04) {
			Return (0x0F)
		} Else {
			Return (0x00)
		}
	}
	Name (_CRS, ResourceTemplate ()
	{
		IO (Decode16, 0x03F8, 0x03F8, 0x08, 0x08)
		IRQNoFlags () {4}
	})
	Name (_PRS, ResourceTemplate ()
	{
		IO (Decode16, 0x03F8, 0x03F8, 0x08, 0x08)
		IRQNoFlags () {4}
	})
}

Device (LPT1) {
	Name (_HID, EISAID ("PNP0400"))
	Name (_UID, 1)
	Method (_STA, 0, NotSerialized) {
		Local0 = FLG0 & 0x04
		If (Local0 == 0x04) {
			Return (0x0F)
		} Else {
			Return (0x00)
		}
	}
	Name (_CRS, ResourceTemplate ()
	{
		IO (Decode16, 0x0378, 0x0378, 0x04, 0x08)
		IRQNoFlags () {5}
	})
	Name (_PRS, ResourceTemplate ()
	{
		IO (Decode16, 0x0378, 0x0378, 0x04, 0x08)
		IRQNoFlags () {5}
	})
}
