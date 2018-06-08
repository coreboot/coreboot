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

/* SMSC SIO10N268 */

Device (SIO1)
{
	Name (_HID, EISAID("PNP0A05"))
	Name (_UID, 1)

	OperationRegion(SIOR, SystemIO, 0x4e, 0x02)
	Field (SIOR, ByteAcc, Nolock, Preserve)
	{
		INDX,	8,
		DATA,	8
	}

	Mutex (SIOM, 0x00)

	Method (READ, 3)
	{
		Acquire (SIOM, 0xffff)
		If (LEqual(Arg0, 0)) {
			Store (0x55, INDX)
			Store (Arg1, INDX)
			Store (DATA, Local1)
			Store (0xaa, INDX)
		}
		And (Local1, Arg2, Local1)
		Release(SIOM)
		Return(Local1)
	}

	Method (WRIT, 3)
	{
		Acquire (SIOM, 0xffff)
		If (LEqual(Arg0, 0)) {
			Store (0x55, INDX)
			Store (Arg1, INDX)
			Store (Arg2, DATA)
			Store (0xaa, INDX)
		}
		Release(SIOM)
	}

	Device (UAR1)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 1)
		Name(_DDN, "COM1")
		Name(_PRW, Package() { 0x08, 0x03 })

		/* Device Status */
		Method (_STA, 0)
		{
			Acquire (SIOM, 0xffff)

			// Is the hardware enabled?
			Store (READ(0, 0x24, 0xff), Local0)
			If (LEqual(Local0, 0)) {
				Return (0xd)
			} Else {
				// Power Enabled?
				Store (READ(0, 0x02, 0x08), Local0)
				If (LEqual(Local0, 0)) {
					Return (0x0d)
				} Else {
					Return (0x0f)
				}
			}
		}

		/* Device Disable */
		Method (_DIS, 0)
		{
			WRIT(0, 0x24, 0x00)

			Store(READ(0, 0x28, 0x0f), Local0)
			WRIT(0, 0x28, Local0)

			Store(READ(0, 0x02, 0xff), Local0)
			Not(0x08, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x02, Local0)
		}

		/* Possible Resource Settings */
		Name(_PRS, ResourceTemplate() {
			StartDependentFn(0, 1) {
				IO(Decode16, 0x3f8, 0x3f8, 0x8, 0x8)
				IRQNoFlags() { 4 }
			} EndDependentFn()
		})

		/* Current Resource Settings */
		Method(_CRS, 0, Serialized)
		{
			Name(NONE, ResourceTemplate() {
				IO(Decode16, 0x000, 0x000, 0x0, 0x1)
				IRQNoFlags() { }
			})

			Name(RSRC, ResourceTemplate() {
				IO(Decode16, 0x3f8, 0x3f8, 0x8, 0x8, _IOA)
				IRQNoFlags(_IRA) { 4 }
			})

			And (_STA(), 0x02, Local0)
			If (LEqual(Local0, 0)) {
				Return(NONE)
			}

			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR1._CRS._IOA._MIN, IOMN)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR1._CRS._IOA._MAX, IOMX)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR1._CRS._IRA._INT, IRQ)

			/* I/O Base */
			Store (READ(0, 0x24, 0xfe), Local0)
			ShiftLeft(Local0, 0x02, Local0)
			Store(Local0, IOMN)
			Store(Local0, IOMX)

			/* Interrupt */
			Store(READ(0, 0x28, 0xf0), Local0)
			ShiftRight(Local0, 4, Local0)
			ShiftLeft(1, Local0, IRQ)
			Return(RSRC)
		}

		/* Set Resource Settings */
		Method(_SRS, 1)
		{
			CreateByteField(Arg0, 0x02, IOLO)
			CreateByteField(Arg0, 0x03, IOHI)
			CreateByteField(Arg0, 0x09, IRQL)

			WRIT(0, 0x24, 0)
			FindSetRightBit(IRQL, Local0)
			Decrement(Local0)
			ShiftLeft(Local0, 4, Local0)

			Store(READ(0, 0x28, 0x0f), Local1)
			Or(Local0, Local1, Local0)
			WRIT(0, 0x28, Local0)

			Store(IOLO, Local0)
			ShiftRight(Local0, 2, Local0)
			And(Local0, 0xfe, Local0)

			Store(IOHI, Local1)
			ShiftLeft(Local1, 6, Local1)
			Or (Local0, Local1, Local0)
			WRIT(0, 0x24, Local0)

			Store(READ(0, 0x02, 0xff), Local0)
			Or(Local0, 0x08, Local0)
			WRIT(0, 0x02, Local0)

			Store(READ(0, 0x07, 0xff), Local0)
			Not(0x40, Local1)
			And (Local0, Local1, Local0)
			WRIT(0, 0x07, Local0)
		}


		/* D0 state - Line drivers are on */
		Method (_PS0, 0)
		{
			Store(READ(0, 0x02, 0xff), Local0)
			Or(Local0, 0x08, Local0)
			WRIT(0, 0x02, Local0)

			Store (READ(0, 0x07, 0xff), Local0)
			Not(0x40, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x07, Local0)
		}

		/* D3 State - Line drivers are off */
		Method(_PS3, 0)
		{
			Store(READ(0, 0x02, 0xff), Local0)
			Not(0x08, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x02, Local0)
		}
	}

	Device (UAR2)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 2)
		Name(_DDN, "COM2")
		Name(_PRW, Package() { 0x08, 0x03 })

		/* Device Status */
		Method (_STA, 0)
		{
			/* IRDA? */
			Store(READ(0, 0x0c, 0x38), Local0)
			If (LNotEqual(Local0, Zero)) {
				Return (0)
			}

			// Is the hardware enabled?
			Store (READ(0, 0x25, 0xff), Local0)
			If (LEqual(Local0, 0)) {
				Return (0xd)
			} Else {
				// Power Enabled?
				Store (READ(0, 0x02, 0x80), Local0)
				If (LEqual(Local0, 0)) {
					Return (0x0d)
				} Else {
					Return (0x0f)
				}
			}
		}

		/* Device Disable */
		Method (_DIS, 0)
		{
			WRIT(0, 0x25, 0x00)

			Store(READ(0, 0x28, 0xf0), Local0)
			WRIT(0, 0x28, Local0)

			Store(READ(0, 0x02, 0xff), Local0)
			Not(0x80, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x02, Local0)
		}

		/* Possible Resource Settings */
		Name(_PRS, ResourceTemplate() {
			StartDependentFn(0, 1) {
				IO(Decode16, 0x2f8, 0x2f8, 0x8, 0x8)
				IRQNoFlags() { 3 }
			} EndDependentFn()
		})

		/* Current Resource Settings */
		Method(_CRS, 0, Serialized)
		{
			Name(NONE, ResourceTemplate() {
				IO(Decode16, 0x000, 0x000, 0x0, 0x1)
				IRQNoFlags() { }
			})

			Name(RSRC, ResourceTemplate() {
				IO(Decode16, 0x2f8, 0x2f8, 0x8, 0x8, _IOB)
				IRQNoFlags(_IRB) { 3 }
			})

			And (_STA(), 0x02, Local0)
			If (LEqual(Local0, 0)) {
				Return(NONE)
			}

			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR2._CRS._IOB._MIN, IOMN)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR2._CRS._IOB._MAX, IOMX)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR2._CRS._IRB._INT, IRQ)

			/* I/O Base */
			Store (READ(0, 0x25, 0xfe), Local0)
			ShiftLeft(Local0, 0x02, Local0)
			Store(Local0, IOMN)
			Store(Local0, IOMX)

			/* Interrupt */
			Store(READ(0, 0x28, 0x0f), Local0)
			ShiftRight(Local0, 4, Local0)
			ShiftLeft(1, Local0, IRQ)
			Return(RSRC)
		}

		/* Set Resource Settings */
		Method(_SRS, 1)
		{
			CreateByteField(Arg0, 0x02, IOLO)
			CreateByteField(Arg0, 0x03, IOHI)
			CreateByteField(Arg0, 0x09, IRQL)

			WRIT(0, 0x25, 0)
			FindSetRightBit(IRQL, Local0)
			Decrement(Local0)

			Store(READ(0, 0x28, 0xf0), Local1)
			Or(Local0, Local1, Local0)
			WRIT(0, 0x28, Local0)

			Store(IOLO, Local0)
			ShiftRight(Local0, 2, Local0)
			And(Local0, 0xfe, Local0)

			Store(IOHI, Local1)
			ShiftLeft(Local1, 6, Local1)
			Or (Local0, Local1, Local0)
			WRIT(0, 0x25, Local0)

			Store(READ(0, 0x0c, 0xff), Local0)
			Not(0x38, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x0c, Local0)

			Store(READ(0, 0x02, 0xff), Local0)
			Or(Local0, 0x80, Local0)
			WRIT(0, 0x02, Local0)

			Store(READ(0, 0x07, 0xff), Local0)
			Not(0x20, Local1)
			And (Local0, Local1, Local0)
			WRIT(0, 0x07, Local0)
		}

		/* D0 state - Line drivers are on */
		Method (_PS0, 0)
		{
			Store(READ(0, 0x02, 0xff), Local0)
			Or(Local0, 0x80, Local0)
			WRIT(0, 0x02, Local0)

			Store (READ(0, 0x07, 0xff), Local0)
			Not(0x20, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x07, Local0)
		}

		/* D3 State - Line drivers are off */
		Method(_PS3, 0)
		{
			Store(READ(0, 0x02, 0xff), Local0)
			Not(0x80, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x02, Local0)
		}
	}

	Device (UAR3)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 3)
		Name(_DDN, "COM3")
		Name(_PRW, Package() { 0x08, 0x03 })

		/* Device Status */
		Method (_STA, 0)
		{
			Acquire (SIOM, 0xffff)

			// Is the hardware enabled?
			Store (READ(0, 0x1b, 0xff), Local0)
			If (LEqual(Local0, 0)) {
				Return (0xd)
			} Else {
				// Power Enabled?
				Store (READ(0, 0x02, 0x02), Local0)
				If (LEqual(Local0, 0)) {
					Return (0x0d)
				} Else {
					Return (0x0f)
				}
			}
		}

		/* Device Disable */
		Method (_DIS, 0)
		{
			WRIT(0, 0x1b, 0x00)

			Store(READ(0, 0x1d, 0x0f), Local0)
			WRIT(0, 0x1d, Local0)

			Store(READ(0, 0x02, 0xff), Local0)
			Not(0x02, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x02, Local0)
		}

		/* Possible Resource Settings */
		Name(_PRS, ResourceTemplate() {
			StartDependentFn(0, 1) {
				IO(Decode16, 0x3e8, 0x3e8, 0x8, 0x8)
				IRQNoFlags() { 5 }
			} EndDependentFn()
		})

		/* Current Resource Settings */
		Method(_CRS, 0, Serialized)
		{
			Name(NONE, ResourceTemplate() {
				IO(Decode16, 0x000, 0x000, 0x0, 0x1)
				IRQNoFlags() { }
			})

			Name(RSRC, ResourceTemplate() {
				IO(Decode16, 0x3e8, 0x3e8, 0x8, 0x8, _IOA)
				IRQNoFlags(_IRA) { 5 }
			})

			And (_STA(), 0x02, Local0)
			If (LEqual(Local0, 0)) {
				Return(NONE)
			}

			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR3._CRS._IOA._MIN, IOMN)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR3._CRS._IOA._MAX, IOMX)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR3._CRS._IRA._INT, IRQ)

			/* I/O Base */
			Store (READ(0, 0x1b, 0xfe), Local0)
			ShiftLeft(Local0, 0x02, Local0)
			Store(Local0, IOMN)
			Store(Local0, IOMX)

			/* Interrupt */
			Store(READ(0, 0x1d, 0xf0), Local0)
			ShiftRight(Local0, 4, Local0)
			ShiftLeft(1, Local0, IRQ)
			Return(RSRC)
		}

		/* Set Resource Settings */
		Method(_SRS, 1)
		{
			CreateByteField(Arg0, 0x02, IOLO)
			CreateByteField(Arg0, 0x03, IOHI)
			CreateByteField(Arg0, 0x09, IRQL)

			WRIT(0, 0x1b, 0)
			FindSetRightBit(IRQL, Local0)
			Decrement(Local0)
			ShiftLeft(Local0, 4, Local0)

			Store(READ(0, 0x1d, 0x0f), Local1)
			Or(Local0, Local1, Local0)
			WRIT(0, 0x1d, Local0)

			Store(IOLO, Local0)
			ShiftRight(Local0, 2, Local0)
			And(Local0, 0xfe, Local0)

			Store(IOHI, Local1)
			ShiftLeft(Local1, 6, Local1)
			Or (Local0, Local1, Local0)
			WRIT(0, 0x1b, Local0)

			Store(READ(0, 0x02, 0xff), Local0)
			Or(Local0, 0x02, Local0)
			WRIT(0, 0x02, Local0)

			Store(READ(0, 0x07, 0xff), Local0)
			Not(0x04, Local1)
			And (Local0, Local1, Local0)
			WRIT(0, 0x07, Local0)
		}


		/* D0 state - Line drivers are on */
		Method (_PS0, 0)
		{
			Store(READ(0, 0x02, 0xff), Local0)
			Or(Local0, 0x02, Local0)
			WRIT(0, 0x02, Local0)

			Store (READ(0, 0x07, 0xff), Local0)
			Not(0x04, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x07, Local0)
		}

		/* D3 State - Line drivers are off */
		Method(_PS3, 0)
		{
			Store(READ(0, 0x02, 0xff), Local0)
			Not(0x02, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x02, Local0)
		}
	}


	Device (UAR4)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 4)
		Name(_DDN, "COM4")
		Name(_PRW, Package() { 0x08, 0x03 })

		/* Device Status */
		Method (_STA, 0)
		{
			Acquire (SIOM, 0xffff)

			// Is the hardware enabled?
			Store (READ(0, 0x1c, 0xff), Local0)
			If (LEqual(Local0, 0)) {
				Return (0xd)
			} Else {
				// Power Enabled?
				Store (READ(0, 0x02, 0x04), Local0)
				If (LEqual(Local0, 0)) {
					Return (0x0d)
				} Else {
					Return (0x0f)
				}
			}
		}

		/* Device Disable */
		Method (_DIS, 0)
		{
			WRIT(0, 0x1c, 0x00)

			Store(READ(0, 0x1d, 0x0f), Local0)
			WRIT(0, 0x1d, Local0)

			Store(READ(0, 0x02, 0xff), Local0)
			Not(0x04, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x02, Local0)
		}

		/* Possible Resource Settings */
		Name(_PRS, ResourceTemplate() {
			StartDependentFn(0, 1) {
				IO(Decode16, 0x2e8, 0x2e8, 0x8, 0x8)
				IRQNoFlags() { 11 }
			} EndDependentFn()
		})

		/* Current Resource Settings */
		Method(_CRS, 0, Serialized)
		{
			Name(NONE, ResourceTemplate() {
				IO(Decode16, 0x000, 0x000, 0x0, 0x1)
				IRQNoFlags() { }
			})

			Name(RSRC, ResourceTemplate() {
				IO(Decode16, 0x2e8, 0x2e8, 0x8, 0x8, _IOA)
				IRQNoFlags(_IRA) { 11 }
			})

			And (_STA(), 0x02, Local0)
			If (LEqual(Local0, 0)) {
				Return(NONE)
			}

			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR4._CRS._IOA._MIN, IOMN)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR4._CRS._IOA._MAX, IOMX)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.UAR4._CRS._IRA._INT, IRQ)

			/* I/O Base */
			Store (READ(0, 0x1c, 0xfe), Local0)
			ShiftLeft(Local0, 0x02, Local0)
			Store(Local0, IOMN)
			Store(Local0, IOMX)

			/* Interrupt */
			Store(READ(0, 0x1d, 0xf0), Local0)
			ShiftRight(Local0, 4, Local0)
			ShiftLeft(1, Local0, IRQ)
			Return(RSRC)
		}

		/* Set Resource Settings */
		Method(_SRS, 1)
		{
			CreateByteField(Arg0, 0x02, IOLO)
			CreateByteField(Arg0, 0x03, IOHI)
			CreateByteField(Arg0, 0x09, IRQL)

			WRIT(0, 0x1c, 0)
			FindSetRightBit(IRQL, Local0)
			Decrement(Local0)
			ShiftLeft(Local0, 4, Local0)

			Store(READ(0, 0x1d, 0x0f), Local1)
			Or(Local0, Local1, Local0)
			WRIT(0, 0x1d, Local0)

			Store(IOLO, Local0)
			ShiftRight(Local0, 2, Local0)
			And(Local0, 0xfe, Local0)

			Store(IOHI, Local1)
			ShiftLeft(Local1, 6, Local1)
			Or (Local0, Local1, Local0)
			WRIT(0, 0x1c, Local0)

			Store(READ(0, 0x02, 0xff), Local0)
			Or(Local0, 0x04, Local0)
			WRIT(0, 0x02, Local0)

			Store(READ(0, 0x07, 0xff), Local0)
			Not(0x08, Local1)
			And (Local0, Local1, Local0)
			WRIT(0, 0x07, Local0)
		}


		/* D0 state - Line drivers are on */
		Method (_PS0, 0)
		{
			Store(READ(0, 0x02, 0xff), Local0)
			Or(Local0, 0x04, Local0)
			WRIT(0, 0x02, Local0)

			Store (READ(0, 0x07, 0xff), Local0)
			Not(0x08, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x07, Local0)
		}

		/* D3 State - Line drivers are off */
		Method(_PS3, 0)
		{
			Store(READ(0, 0x02, 0xff), Local0)
			Not(0x04, Local1)
			And(Local0, Local1, Local0)
			WRIT(0, 0x02, Local0)
		}
	}

}
