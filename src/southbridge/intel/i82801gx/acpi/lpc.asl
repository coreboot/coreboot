/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// Intel LPC Bus Device  - 0:1f.0

Device (LPCB)
{
	Name(_ADR, 0x001f0000)

	OperationRegion(LPC0, PCI_Config, 0x00, 0x100)
	Field (LPC0, AnyAcc, NoLock, Preserve)
	{
		Offset (0x40),
		PMBS,	16,	// PMBASE
		Offset (0x60),	// Interrupt Routing Registers
		PRTA,	8,
		PRTB,	8,
		PRTC,	8,
		PRTD,	8,
		Offset (0x68),
		PRTE,	8,
		PRTF,	8,
		PRTG,	8,
		PRTH,	8,

		Offset (0x80),	// IO Decode Ranges
		IOD0,	8,
		IOD1,	8,

		Offset (0xf0),	// RCBA
		RCEN,	1,
		,	13,
		RCBA,	18,
	}

	#include "irqlinks.asl"

	#include "acpi/ec.asl"

	Device (DMAC)		// DMA Controller
	{
		Name(_HID, EISAID("PNP0200"))
		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x00, 0x00, 0x01, 0x20)
			IO (Decode16, 0x81, 0x81, 0x01, 0x11)
			IO (Decode16, 0x93, 0x93, 0x01, 0x0d)
			IO (Decode16, 0xc0, 0xc0, 0x01, 0x20)
			DMA (Compatibility, NotBusMaster, Transfer8_16) { 4 }
		})
	}

	Device (FWH)		// Firmware Hub
	{
		Name (_HID, EISAID("INT0800"))
		Name (_CRS, ResourceTemplate()
		{
			Memory32Fixed(ReadOnly, 0xff000000, 0x01000000)
		})
	}

	Device (HPET)
	{
		Name (_HID, EISAID("PNP0103"))
		Name (_CID, 0x010CD041)

		Name(BUF0, ResourceTemplate()
		{
			Memory32Fixed(ReadOnly, 0xfed00000, 0x400, FED0)
		})

		Method (_STA, 0)	// Device Status
		{
			If (HPTE) {
				// Note: Ancient versions of Windows don't want
				// to see the HPET in order to work right
				If (LGreaterEqual(OSYS, 2001)) {
					Return (0xf)	// Enable and show device
				} Else {
					Return (0xb)	// Enable and don't show device
				}
			}

			Return (0x0)	// Not enabled, don't show.
		}

		Method (_CRS, 0, Serialized) // Current resources
		{
			If (HPTE) {
				CreateDWordField(BUF0, \_SB.PCI0.LPCB.HPET.FED0._BAS, HPT0)
				If (Lequal(HPAS, 1)) {
					Store(0xfed01000, HPT0)
				}

				If (Lequal(HPAS, 2)) {
					Store(0xfed02000, HPT0)
				}

				If (Lequal(HPAS, 3)) {
					Store(0xfed03000, HPT0)
				}
			}

			Return (BUF0)
		}
	}

	Device(PIC)	// 8259 Interrupt Controller
	{
		Name(_HID,EISAID("PNP0000"))
		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x20, 0x20, 0x01, 0x02)
			IO (Decode16, 0x24, 0x24, 0x01, 0x02)
			IO (Decode16, 0x28, 0x28, 0x01, 0x02)
			IO (Decode16, 0x2c, 0x2c, 0x01, 0x02)
			IO (Decode16, 0x30, 0x30, 0x01, 0x02)
			IO (Decode16, 0x34, 0x34, 0x01, 0x02)
			IO (Decode16, 0x38, 0x38, 0x01, 0x02)
			IO (Decode16, 0x3c, 0x3c, 0x01, 0x02)
			IO (Decode16, 0xa0, 0xa0, 0x01, 0x02)
			IO (Decode16, 0xa4, 0xa4, 0x01, 0x02)
			IO (Decode16, 0xa8, 0xa8, 0x01, 0x02)
			IO (Decode16, 0xac, 0xac, 0x01, 0x02)
			IO (Decode16, 0xb0, 0xb0, 0x01, 0x02)
			IO (Decode16, 0xb4, 0xb4, 0x01, 0x02)
			IO (Decode16, 0xb8, 0xb8, 0x01, 0x02)
			IO (Decode16, 0xbc, 0xbc, 0x01, 0x02)
			IO (Decode16, 0x4d0, 0x4d0, 0x01, 0x02)
			IRQNoFlags () { 2 }
		})
	}

	Device(MATH)	// FPU
	{
		Name (_HID, EISAID("PNP0C04"))
		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, 0xf0, 0xf0, 0x01, 0x01)
			IRQNoFlags() { 13 }
		})
	}

	Device(LDRC)	// LPC device: Resource consumption
	{
		Name (_HID, EISAID("PNP0C02"))
		Name (_UID, 2)
		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x2e, 0x2e, 0x1, 0x02)		// First SuperIO
			IO (Decode16, 0x4e, 0x4e, 0x1, 0x02)		// Second SuperIO
			IO (Decode16, 0x61, 0x61, 0x1, 0x01)		// NMI Status
			IO (Decode16, 0x63, 0x63, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0x65, 0x65, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0x67, 0x67, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0x80, 0x80, 0x1, 0x01)		// Port 80 Post
			IO (Decode16, 0x92, 0x92, 0x1, 0x01)		// CPU Reserved
			IO (Decode16, 0xb2, 0xb2, 0x1, 0x02)		// SWSMI
			IO (Decode16, 0x800, 0x800, 0x1, 0x10)		// ACPI I/O trap
			IO (Decode16, DEFAULT_PMBASE, DEFAULT_PMBASE, 0x1, 0x80)	// ICH7-M ACPI
			IO (Decode16, DEFAULT_GPIOBASE, DEFAULT_GPIOBASE, 0x1, 0x40)	// ICH7-M GPIO
		})
	}

	Device (RTC)	// Real Time Clock
	{
		Name (_HID, EISAID("PNP0B00"))
		Name (_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x70, 0x70, 1, 8)
// Disable as Windows doesn't like it, and systems don't seem to use it.
//			IRQNoFlags() { 8 }
		})
	}

	Device (TIMR)	// Intel 8254 timer
	{
		Name(_HID, EISAID("PNP0100"))
		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x40, 0x40, 0x01, 0x04)
			IO (Decode16, 0x50, 0x50, 0x10, 0x04)
			IRQNoFlags() {0}
		})
	}

	#include "acpi/superio.asl"

	Device (PS2K)		// Keyboard
	{
		Name(_HID, EISAID("PNP0303"))
		Name(_CID, EISAID("PNP030B"))

		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x60, 0x60, 0x01, 0x01)
			IO (Decode16, 0x64, 0x64, 0x01, 0x01)
			IRQ (Edge, ActiveHigh, Exclusive) { 0x01 } // IRQ 1
		})

		Method (_STA, 0)
		{
			Return (0xf)
		}
	}

	Device (PS2M)		// Mouse
	{
		Name(_HID, EISAID("PNP0F13"))
		Name(_CRS, ResourceTemplate()
		{
			IRQ (Edge, ActiveHigh, Exclusive) { 0x0c } // IRQ 12
		})

		Method(_STA, 0)
		{
			Return (0xf)
		}
	}

#ifdef ENABLE_FDC
	Device (FDC0)		// Floppy controller
	{
		Name (_HID, EisaId ("PNP0700"))
		Method (_STA, 0, NotSerialized)
		{
			Return (0x0f) // FIXME
		}

		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x03F0, 0x03F0, 0x01, 0x06)
			IO (Decode16, 0x03F7, 0x03F7, 0x01, 0x01)
			IRQNoFlags () {6}
			DMA (Compatibility, NotBusMaster, Transfer8) {2}
		})

		Name(_PRS, ResourceTemplate()
		{
			IO (Decode16, 0x03F0, 0x03F0, 0x01, 0x06)
			IO (Decode16, 0x03F7, 0x03F7, 0x01, 0x01)
			IRQNoFlags () {6}
			DMA (Compatibility, NotBusMaster, Transfer8) {2}
		})

	}
#endif
}
