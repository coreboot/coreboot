/* SPDX-License-Identifier: GPL-2.0-only */

/* Helper package for determining IO, DMA, IRQ location according to LDN */
Name (DCAT, Package (0x10) {
	0x07,	/* UARTA */
	0x08,	/* UARTB */
	0x11,	/* LPT */
	0x0B,	/* Floppy */
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	0xFF,
	1,	/* KBC */
	0xFF,
	0xFF,
	0xFF,
	1,	/* KBC */
	0xFF
})

Method (CGLD, 1, NotSerialized)
{
	Return (DerefOf (DCAT [Arg0]))
}

/* Return Parallel port mode*/
Method (LPTM, 1, NotSerialized)
{
	ENTER_CONFIG_MODE (CGLD (Arg0))
	Local0 = (OPT0 & 0x02)
	EXIT_CONFIG_MODE ()
	Return (Local0)
}

/* Device Status */
Method (DSTA, 1, NotSerialized)
{
	ENTER_CONFIG_MODE (CGLD (Arg0))
	Local0 = PNP_DEVICE_ACTIVE
	If (Local0 == 0xFF)
	{
		Return (0)
	}

	Local0 &= 1
	If (Arg0 < 0x10)
	{
		IOST |= (Local0 << Arg0)
	}

	If (Local0)
	{
		Return (DEVICE_PRESENT_ACTIVE)
	}
	ElseIf ((1 << Arg0) & IOST)
	{
		Return (DEVICE_PRESENT_INACTIVE)
	}
	Else
	{
		Return (0)
	}

	EXIT_CONFIG_MODE ()
}

Method (DCNT, 2, NotSerialized)
{
	ENTER_CONFIG_MODE (CGLD (Arg0))
	PNP_DEVICE_ACTIVE = Arg1
	EXIT_CONFIG_MODE ()
}

/* Resource templates for SIO LDNs */
Name (CRS1, ResourceTemplate ()
{
	IO (Decode16,
		0x0000,
		0x0000,
		0x01,
		0x00,
		_Y16)
	IRQ (Edge, ActiveHigh, Exclusive, _Y14) {}
	DMA (Compatibility, NotBusMaster, Transfer8, _Y15) {}
})

CreateWordField (CRS1, \_SB.PCI0.LPCB.SIO1._Y14._INT, IRQM)
CreateByteField (CRS1, \_SB.PCI0.LPCB.SIO1._Y15._DMA, DMAM)
CreateWordField (CRS1, \_SB.PCI0.LPCB.SIO1._Y16._MIN, IO11)
CreateWordField (CRS1, \_SB.PCI0.LPCB.SIO1._Y16._MAX, IO12)
CreateByteField (CRS1, \_SB.PCI0.LPCB.SIO1._Y16._LEN, LEN1)

Name (CRS2, ResourceTemplate ()
{
	IO (Decode16,
		0x0000,
		0x0000,
		0x01,
		0x00,
		_Y19)
	IO (Decode16,
		0x0000,
		0x0000,
		0x01,
		0x00,
		_Y1A)
	IRQ (Edge, ActiveHigh, Exclusive, _Y17) {}
	DMA (Compatibility, NotBusMaster, Transfer8, _Y18) {}
})

CreateWordField (CRS2, \_SB.PCI0.LPCB.SIO1._Y17._INT, IRQE)
CreateByteField (CRS2, \_SB.PCI0.LPCB.SIO1._Y18._DMA, DMAE)
CreateWordField (CRS2, \_SB.PCI0.LPCB.SIO1._Y19._MIN, IO21)
CreateWordField (CRS2, \_SB.PCI0.LPCB.SIO1._Y19._MAX, IO22)
CreateByteField (CRS2, \_SB.PCI0.LPCB.SIO1._Y19._LEN, LEN2)
CreateWordField (CRS2, \_SB.PCI0.LPCB.SIO1._Y1A._MIN, IO31)
CreateWordField (CRS2, \_SB.PCI0.LPCB.SIO1._Y1A._MAX, IO32)
CreateByteField (CRS2, \_SB.PCI0.LPCB.SIO1._Y1A._LEN, LEN3)

/* Read IO resource */
Method (GIOB, 1, NotSerialized)
{
	If (CGLD (Arg0) == 0x07)	/* UARTA */
	{
		SWITCH_LDN (SUPERIO_LPC_LDN)
		Local0 = (CR6B << 0x08)
		Local0 |= CR6A
		Return (Local0)
	}

	If (CGLD (Arg0) == 0x08)	/* UARTB */
	{
		SWITCH_LDN (SUPERIO_LPC_LDN)
		Local0 = (CR6F << 0x08)
		Local0 |= CR6E
		Return (Local0)
	}

	If (CGLD (Arg0) == 0x11)	/* LPT */
	{
		SWITCH_LDN (SUPERIO_LPC_LDN)
		Local0 = (CR83 << 0x08)
		Local0 |= CR82
		Return (Local0)
	}

	If (CGLD (Arg0) == 0x0B)	/* Floppy */
	{
		SWITCH_LDN (SUPERIO_LPC_LDN)
		Local0 = (CR7F << 0x08)
		Local0 |= CR7E
		Return (Local0)
	}

	Return (0)
}

/* Read IRQ resource */
Method (GIRQ, 1, NotSerialized)
{
	SWITCH_LDN (SUPERIO_LPC_LDN)
	Local0 = 0x0F	/* 15 IRQ regs, 1 for each IRQ number */
	While (Local0)
	{
		Local1 = (0x40 + Local0) /* IRQ regs begin at offset 0x40 */
		PNP_ADDR_REG = Local1
		Local1 = PNP_DATA_REG
		If (CGLD (Arg0) == Local1)
		{
			Local1 = 1
			Local0 = (Local1 << Local0)
			Return (Local0)
		}

		Local0--
	}

	Return (0xFF)
}

/* Read DMA resource */
Method (GDMA, 1, NotSerialized)
{
	SWITCH_LDN (SUPERIO_LPC_LDN)
	Local0 = 0x03	/* Only DMA Channels 0-3 */
	While (Local0)
	{
		Local1 = (Local0 << 1)
		Local1 += 0x51	/* DMA regs begin at offset 0x50 */
		PNP_ADDR_REG = Local1
		Local1 = PNP_DATA_REG
		If ((0x80 | CGLD (Arg0)) == Local1)
		{
			Local1 = 1
			Local0 = (Local1 << Local0)
			Return (Local0)
		}

		Local0--
	}

	Return (0xFF)
}

/* Set IO resource */
Method (STIO, 2, NotSerialized)
{
	SWITCH_LDN (SUPERIO_LPC_LDN)
	Local0 = (Arg1 & 0xFF)
	PNP_ADDR_REG = Arg0
	PNP_DATA_REG = Local0
	Local0 = (Arg1 >> 0x08)
	Local1 = (Arg0 + 1)
	PNP_ADDR_REG = Local1
	PNP_DATA_REG = Local0
}

/* Set IRQ resource */
Method (SIRQ, 2, NotSerialized)
{
	SWITCH_LDN (SUPERIO_LPC_LDN)
	FindSetRightBit (Arg1, Local0)
	Local0 -= 1
	Local1 = 0x0F
	While (Local1)
	{
		Local2 = (0x40 + Local1)
		PNP_ADDR_REG = Local2
		Local3 = PNP_DATA_REG
		If (CGLD (Arg0) == Local3)
		{
			If (Local0 != Local1)
			{
				PNP_ADDR_REG = Local2
				PNP_DATA_REG = 0xFF
				Break
			}
			Else
			{
				Return (0)
			}
		}

		Local1--
	}

	Local0 += 0x40
	PNP_ADDR_REG = Local0
	PNP_DATA_REG = CGLD (Arg0)
	Return (0xFF)
}

/* Set DMA resource */
Method (SDMA, 2, NotSerialized)
{
	SWITCH_LDN (SUPERIO_LPC_LDN)
	FindSetRightBit (Arg1, Local0)
	Local0 -= 1
	Local1 = 0x03
	While (Local1)
	{
		Local2 = (Local1 << 1)
		Local3 = (0x51 + Local2)
		PNP_ADDR_REG = Local3
		Local4 = PNP_DATA_REG
		If ((0x80 | CGLD (Arg0)) == Local4)
		{
			If (Local0 != Local1)
			{
				PNP_ADDR_REG = Local3
				PNP_DATA_REG = 0
				Break
			}
			Else
			{
				Return (0)
			}
		}

		Local1--
	}

	Local0 <<= 1
	Local0 += 0x51
	PNP_ADDR_REG = Local0
	PNP_DATA_REG = (0x80 | CGLD (Arg0))
	Return (0)
}

/* Device Current Resource Settings */
Method (DCRS, 2, NotSerialized)
{
	If (CGLD (Arg0) == 0x07)	/* UARTA resources */
	{
		ENTER_CONFIG_MODE (SUPERIO_LPC_LDN)
		IO11 = GIOB (Arg0)
		IO12 = IO11
		LEN1 = 0x08
		IRQM = GIRQ (Arg0)
		If ((GDMA (Arg0) > 0x03) || (Arg1 == 0))
		{
			DMAM = 0
		}
		Else
		{
			DMAM = GDMA (Arg0)
		}

		EXIT_CONFIG_MODE ()
		Return (CRS1)
	}

	If (CGLD (Arg0) == 0x08)	/* UARTB resources */
	{
		ENTER_CONFIG_MODE (SUPERIO_LPC_LDN)
		IO11 = GIOB (Arg0)
		IO12 = IO11
		LEN1 = 0x08
		IRQM = GIRQ (Arg0)
		If ((GDMA (Arg0) > 0x03) || (Arg1 == 0))
		{
			DMAM = 0
		}
		Else
		{
			DMAM = GDMA (Arg0)
		}

		EXIT_CONFIG_MODE ()
		Return (CRS1)
	}

	If (CGLD (Arg0) == 0x11)	/* LPT resources */
	{
		If (LPTM (Arg0))
		{
			ENTER_CONFIG_MODE (SUPERIO_LPC_LDN)
			IO21 = GIOB (Arg0)
			IO22 = IO21
			IO31 = (IO21 + 0x0400)
			IO32 = IO31
			If ((IO21 & 0xFF) == 0xBC)
			{
				LEN2 = 0x04
				LEN3 = 0x04
			}
			Else
			{
				LEN2 = 0x08
				LEN3 = 0x04
			}

			IRQE = GIRQ (Arg0)
			If ((GDMA (Arg0) > 0x03) || (Arg1 == 0))
			{
				DMAM = 0
			}
			Else
			{
				DMAE = GDMA (Arg0)
			}

			EXIT_CONFIG_MODE ()
			Return (CRS2) /* \_SB_.PCI0.LPCB.SIO1.CRS2 */
		}
		Else
		{
			ENTER_CONFIG_MODE (SUPERIO_LPC_LDN)
			IO11 = GIOB (Arg0)
			IO12 = IO11 /* \_SB_.PCI0.LPCB.SIO1.IO11 */
			If ((IO11 & 0xFF) == 0xBC)
			{
				LEN1 = 0x04
			}
			Else
			{
				LEN1 = 0x08
			}

			IRQM = GIRQ (Arg0)
			EXIT_CONFIG_MODE ()
			Return (CRS1) /* \_SB_.PCI0.LPCB.SIO1.CRS1 */
		}
	}

	If (CGLD (Arg0) == 0x0B)	/* Floppy resources */
	{
		ENTER_CONFIG_MODE (SUPERIO_LPC_LDN)
		IO21 = GIOB (Arg0)
		IO22 = IO21 /* \_SB_.PCI0.LPCB.SIO1.IO21 */
		LEN2 = 0x06
		IO31 = (IO21 + 0x07)
		IO32 = IO31 /* \_SB_.PCI0.LPCB.SIO1.IO31 */
		LEN3 = 1
		IRQE = GIRQ (Arg0)
		If ((GDMA (Arg0) > 0x03) || (Arg1 == 0))
		{
			DMAM = 0
		}
		Else
		{
			DMAE = GDMA (Arg0)
		}

		EXIT_CONFIG_MODE ()
		Return (CRS2) /* \_SB_.PCI0.LPCB.SIO1.CRS2 */
	}

	Return (CRS1) /* \_SB_.PCI0.LPCB.SIO1.CRS1 */
}
