/* SPDX-License-Identifier: GPL-2.0-only */

/* SMSC LPC47N227 */

Device (SIO1)
{
	Name (_HID, EISAID("PNP0A05"))
	Name (_UID, 1)

	OperationRegion(SIOR, SystemIO, 0x2e, 0x02)
	Field (SIOR, ByteAcc, Nolock, Preserve)
	{
		INDX,	8,
		DATA,	8
	}

	Mutex (SIOM, 0x00)

	Method (READ, 3)
	{
		Acquire (SIOM, 0xffff)
		If (Arg0 == 0) {
			INDX = 0x55
			INDX = Arg1
			Local1 = DATA
			INDX = 0xaa
		}
		Local1 &= Arg2
		Release(SIOM)
		Return(Local1)
	}

	Method (WRIT, 3)
	{
		Acquire (SIOM, 0xffff)
		If (Arg0 == 0) {
			INDX = 0x55
			INDX = Arg1
			DATA = Arg2
			INDX = 0xaa
		}
		Release(SIOM)
	}

	Device (COMA)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 1)
		Name(_PRW, Package() { 0x08, 0x03 })

		/* Device Status */
		Method (_STA, 0)
		{
			// Device disabled by coreboot?
			If (CMAP == 0) {
				Return (0)
			}

			// Is the hardware enabled?
			Local0 = READ (0, 0x24, 0xff)
			If (Local0 == 0) {
				Return (0xd)
			} Else {
				// Power Enabled?
				Local0 = READ (0, 0x02, 0x08)
				If (Local0 == 0) {
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

			Local0 = READ (0, 0x28, 0x0f)
			WRIT(0, 0x28, Local0)

			Local0 = READ(0, 0x02, 0xff)
			Local1 = ~0x08
			Local0 &= Local1
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
		Method(_CRS, 0)
		{
			Name(NONE, ResourceTemplate() {
				IO(Decode16, 0x000, 0x000, 0x0, 0x1)
				IRQNoFlags() { }
			})

			Name(RSRC, ResourceTemplate() {
				IO(Decode16, 0x3f8, 0x3f8, 0x8, 0x8, _IOA)
				IRQNoFlags(_IRA) { 4 }
			})

			Local0 = _STA() & 0x02
			If (Local0 == 0) {
				Return(NONE)
			}

			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.COMA._CRS._IOA._MIN, IOMN)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.COMA._CRS._IOA._MAX, IOMX)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.COMA._CRS._IRA._INT, IRQ)

			/* I/O Base */
			Local0 = READ (0, 0x24, 0xfe)
			Local0 <<= 2
			IOMN = Local0
			IOMX = Local0

			/* Interrupt */
			Local0 = READ (0, 0x28, 0xf0)
			Local0 >>= 4
			IRQ = 1 << Local0
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
			Local0--
			Local0 <<= 4

			Local1 = READ (0, 0x28, 0x0f)
			Local0 |= Local1
			WRIT(0, 0x28, Local0)

			Local0 = IOLO
			Local0 >>= 2
			Local0 &= 0xfe

			Local1 = IOHI
			Local1 <<= 6
			Local0 |= Local1
			WRIT(0, 0x24, Local0)

			Local0 = READ(0, 0x02, 0xff)
			Local0 |= 0x08
			WRIT(0, 0x02, Local0)

			Local0 = READ (0, 0x07, 0xff)
			Local1 = ~0x40
			Local0 &= Local1
			WRIT(0, 0x07, Local0)
		}


		/* D0 state - Line drivers are on */
		Method (_PS0, 0)
		{
			Local0 = READ(0, 0x02, 0xff)
			Local0 |= 0x08
			WRIT(0, 0x02, Local0)

			Local0 = READ (0, 0x07, 0xff)
			Local1 = ~0x40
			Local0 &= Local1
			WRIT(0, 0x07, Local0)
		}

		/* D3 State - Line drivers are off */
		Method(_PS3, 0)
		{
			Local0 = READ(0, 0x02, 0xff)
			Local1 = ~0x08
			Local0 &= Local1
			WRIT(0, 0x02, Local0)
		}
	}

	Device (COMB)
	{
		Name(_HID, EISAID("PNP0501"))
		Name(_UID, 2)
		Name(_PRW, Package() { 0x08, 0x03 })

		/* Device Status */
		Method (_STA, 0)
		{
			// Device disabled by coreboot?
			If (CMBP == 0) {
				Return (0)
			}

			/* IRDA? */
			Local0 = READ (0, 0x0c, 0x38)
			If (Local0 != 0) {
				Return (0)
			}

			// Is the hardware enabled?
			Local0 = READ (0, 0x25, 0xff)
			If (Local0 == 0) {
				Return (0xd)
			} Else {
				// Power Enabled?
				Local0 = READ (0, 0x02, 0x80)
				If (Local0 == 0) {
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

			Local0 = READ (0, 0x28, 0xf0)
			WRIT(0, 0x28, Local0)

			Local0 = READ(0, 0x02, 0xff)
			Local1 = ~0x80
			Local0 &= Local1
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
		Method(_CRS, 0)
		{
			Name(NONE, ResourceTemplate() {
				IO(Decode16, 0x000, 0x000, 0x0, 0x1)
				IRQNoFlags() { }
			})

			Name(RSRC, ResourceTemplate() {
				IO(Decode16, 0x2f8, 0x2f8, 0x8, 0x8, _IOB)
				IRQNoFlags(_IRB) { 3 }
			})

			Local0 = _STA() & 0x02
			If (Local0 == 0) {
				Return(NONE)
			}

			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.COMB._CRS._IOB._MIN, IOMN)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.COMB._CRS._IOB._MAX, IOMX)
			CreateWordField(RSRC,
				\_SB.PCI0.LPCB.SIO1.COMB._CRS._IRB._INT, IRQ)

			/* I/O Base */
			Local0 = READ (0, 0x25, 0xfe)
			Local0 <<= 2
			IOMN = Local0
			IOMX = Local0

			/* Interrupt */
			Local0 = READ (0, 0x28, 0x0f)
			IRQ = 1 << Local0
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
			Local0--

			Local1 = READ (0, 0x28, 0xf0)
			Local0 |= Local1
			WRIT(0, 0x28, Local0)

			Local0 = IOLO
			Local0 >>= 2
			Local0 &= 0xfe

			Local1 = IOHI
			Local1 <<= 6
			Local0 |= Local1
			WRIT(0, 0x25, Local0)

			Local0 = READ (0, 0x0c, 0xff)
			Local1 = ~0x38
			Local0 &= Local1
			WRIT(0, 0x0c, Local0)

			Local0 = READ(0, 0x02, 0xff)
			Local0 |= 0x80
			WRIT(0, 0x02, Local0)

			Local0 = READ (0, 0x07, 0xff)
			Local1 = ~0x20
			Local0 &= Local1
			WRIT(0, 0x07, Local0)
		}

		/* D0 state - Line drivers are on */
		Method (_PS0, 0)
		{
			Local0 = READ(0, 0x02, 0xff)
			Local0 |= 0x80
			WRIT(0, 0x02, Local0)

			Local0 = READ (0, 0x07, 0xff)
			Local1 = ~0x20
			Local0 &= Local1
			WRIT(0, 0x07, Local0)
		}

		/* D3 State - Line drivers are off */
		Method(_PS3, 0)
		{
			Local0 = READ(0, 0x02, 0xff)
			Local1 = ~0x80
			Local0 &= Local1
			WRIT(0, 0x02, Local0)
		}
	}
}
