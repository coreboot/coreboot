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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

// Intel LPC Bus Device  - 0:1f.0

Device (LPCB)
{
	Name(_ADR, 0x001f0000)

	OperationRegion(LPC0, PCI_Config, 0x00, 0x100)
	Field (LPC0, AnyAcc, NoLock, Preserve)
	{
		Offset (0x3),
		DIDH,	8,	// Device ID High Byte
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

		Offset (0xb8),	// GPIO Routing Control
		GR00,	 2,
		GR01,	 2,
		GR02,	 2,
		GR03,	 2,
		GR04,	 2,
		GR05,	 2,
		GR06,	 2,
		GR07,	 2,
		GR08,	 2,
		GR09,	 2,
		GR10,	 2,
		GR11,	 2,
		GR12,	 2,
		GR13,	 2,
		GR14,	 2,
		GR15,	 2,

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

		Name (RBUF, ResourceTemplate()
		{
			IO (Decode16, 0x2e, 0x2e, 0x1, 0x02) // First SuperIO
			IO (Decode16, 0x4e, 0x4e, 0x1, 0x02) // Second SuperIO
			IO (Decode16, 0x61, 0x61, 0x1, 0x01) // NMI Status
			IO (Decode16, 0x63, 0x63, 0x1, 0x01) // CPU Reserved
			IO (Decode16, 0x65, 0x65, 0x1, 0x01) // CPU Reserved
			IO (Decode16, 0x67, 0x67, 0x1, 0x01) // CPU Reserved
			IO (Decode16, 0x80, 0x80, 0x1, 0x01) // Port 80 Post
			IO (Decode16, 0x92, 0x92, 0x1, 0x01) // CPU Reserved
			IO (Decode16, 0xb2, 0xb2, 0x1, 0x02) // SWSMI
			IO (Decode16, DEFAULT_PMBASE, DEFAULT_PMBASE,
			    0x1, 0xff)

			// GPIO region may be 128 bytes or 4096 bytes
			IO (Decode16, DEFAULT_GPIOBASE, DEFAULT_GPIOBASE,
			    0x1, 0x00, GPR1)
			IO (Decode16, 0x0000, 0x0000, 0x1, 0x00, GPR2)
			IO (Decode16, 0x0000, 0x0000, 0x1, 0x00, GPR3)
			IO (Decode16, 0x0000, 0x0000, 0x1, 0x00, GPR4)
		})

		Method (_CRS, 0, NotSerialized)
		{
			CreateByteField (^RBUF, ^GPR1._LEN, R1LN)
			CreateByteField (^RBUF, ^GPR2._LEN, R2LN)
			CreateByteField (^RBUF, ^GPR3._LEN, R3LN)
			CreateByteField (^RBUF, ^GPR4._LEN, R4LN)

			CreateWordField (^RBUF, ^GPR1._MIN, R1MN)
			CreateWordField (^RBUF, ^GPR2._MIN, R2MN)
			CreateWordField (^RBUF, ^GPR3._MIN, R3MN)
			CreateWordField (^RBUF, ^GPR4._MIN, R4MN)

			CreateWordField (^RBUF, ^GPR1._MAX, R1MX)
			CreateWordField (^RBUF, ^GPR2._MAX, R2MX)
			CreateWordField (^RBUF, ^GPR3._MAX, R3MX)
			CreateWordField (^RBUF, ^GPR4._MAX, R4MX)

			// Update GPIO region for LynxPoint-LP
			If (\ISLP ()) {
				// LynxPoint-LP
				Store (R1MN, Local0)

				// Update GPIO bank 1
				Store (Local0, R1MN)
				Store (Local0, R1MX)
				Store (0xff, R1LN)

				// Update GPIO bank 2
				Add (Local0, 0x100, Local0)
				Store (Local0, R2MN)
				Store (Local0, R2MX)
				Store (0xff, R2LN)

				// Update GPIO bank 3
				Add (Local0, 0x100, Local0)
				Store (Local0, R3MN)
				Store (Local0, R3MN)
				Store (0xff, R3LN)

				// Update GPIO bank 4
				Add (Local0, 0x100, Local0)
				Store (Local0, R4MN)
				Store (Local0, R4MN)
				Store (0xff, R4LN)
			} Else {
				// LynxPoint-H
				// Update GPIO region length
				Store (DEFAULT_GPIOSIZE, R1LN)
			}
			Return (RBUF)
		}
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

#ifdef ENABLE_TPM
	Device (TPM)		// Trusted Platform Module
	{
		Name(_HID, EISAID("IFX0102"))
		Name(_CID, 0x310cd041)
		Name(_UID, 1)

		Method(_STA, 0)
		{
			If (TPMP) {
				Return (0xf)
			}
			Return (0x0)
		}

		Name(_CRS, ResourceTemplate() {
			IO (Decode16, 0x2e, 0x2e, 0x01, 0x02)
			IO (Decode16, 0x6f0, 0x6f0, 0x01, 0x10)
			Memory32Fixed (ReadWrite, 0xfed40000, 0x5000)
			IRQ (Edge, Activehigh, Exclusive) { 6 }
		})
	}
#endif
}
