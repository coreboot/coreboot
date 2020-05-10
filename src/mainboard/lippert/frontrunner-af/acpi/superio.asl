/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * SuperI/O devices
 */

/* PS/2 Keyboard */
Device(KBC) {
	Name(_HID, EISAID("PNP0303"))
	Name(_CRS, ResourceTemplate() {
		IO(Decode16, 0x0060, 0x0060, 1, 1)
		IO(Decode16, 0x0064, 0x0064, 1, 1)
		IRQNoFlags(){1}
	})
}

/* PS/2 Mouse */
Device(PS2M) {
	Name(_HID, EISAID("PNP0F13"))
	Name(_CRS, ResourceTemplate() {
		IRQNoFlags(){12}
	})
}
