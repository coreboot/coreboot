/*
 * SuperI/O devices
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 LiPPERT ADLINK Technology GmbH
 * (Written by Jens Rottmann <JRottmann@LiPPERTembedded.de> for LiPPERT)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
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
