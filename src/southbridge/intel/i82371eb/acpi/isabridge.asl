/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Nick Barker <Nick.Barker9@btinternet.com>
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
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

/*
 * ISA portions taken from QEMU acpi-dsdt.dsl.
 */

// Intel LPC Bus Device  - 0:4.0
Device (LPCB)
{
	Name(_ADR, 0x00040000)

	OperationRegion(PCIC, PCI_Config, 0x00, 0x100)

	/* PS/2 keyboard (seems to be important for WinXP install) */
	Device (KBD)
	{
		Name (_HID, EisaId ("PNP0303"))
		Method (_STA, 0, NotSerialized)
		{
			Return (0x0f)
		}
		Method (_CRS, 0, NotSerialized)
		{
			Name (TMP, ResourceTemplate () {
				IO (Decode16, 0x0060, 0x0060, 0x01, 0x01)
				IO (Decode16, 0x0064, 0x0064, 0x01, 0x01)
				IRQNoFlags () {1}
			})
			Return (TMP)
		}
	}

	/* PS/2 mouse */
	Device (MOU)
	{
		Name (_HID, EisaId ("PNP0F13"))
		Method (_STA, 0, NotSerialized)
		{
			Return (0x0f)
		}
		Method (_CRS, 0, NotSerialized)
		{
			Name (TMP, ResourceTemplate () {
			     IRQNoFlags () {12}
			})
			Return (TMP)
		}
	}

	/* PS/2 floppy controller */
	Device (FDC0)
	{
		Name (_HID, EisaId ("PNP0700"))
		Method (_STA, 0, NotSerialized)
		{
			Return (0x0f)
		}
		Method (_CRS, 0, NotSerialized)
		{
			Name (BUF0, ResourceTemplate () {
				IO (Decode16, 0x03F2, 0x03F2, 0x00, 0x04)
				IO (Decode16, 0x03F7, 0x03F7, 0x00, 0x01)
				IRQNoFlags () {6}
				DMA (Compatibility, NotBusMaster, Transfer8) {2}
			})
			Return (BUF0)
		}
	}
}

Device(MBRS) {
	Name (_HID, EisaId ("PNP0C02"))
	Name (_UID, 0x01)

	External(_CRS) /* Resource Template in SSDT */
}
