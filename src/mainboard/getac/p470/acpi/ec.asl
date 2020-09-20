/* SPDX-License-Identifier: GPL-2.0-only */

Device(EC0)
{
	Name (_HID, EISAID("PNP0C09"))
	Name (_UID, 1)

	Name (_GPE, 23)	// GPI07 / GPE23 -> Runtime SCI

	OperationRegion(ERAM, EmbeddedControl, 0x00, 0x37)
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		MODE,	  1,	// Thermal Policy (Quiet/Perform)
		FAN,	  1,	// Fan Power (On/Off)
		LIDS,	  1,	// LID Status (0=Open/1=Close)
		LSW0,	  1,	// LCD Power Switch

		BTYP,	  1,	// Battery Type (0=LiIon/1=NiMh)
		MWKE,	  1,	// Enable Wakeup from RI
		ADP,	  1,	// AC Adapter (0=Offline/1=Online)
		BAT,	  1,	// Battery (0=Not Present/1=Present)
		SLPT,	  3,	// Set Sleep Type on SLP enter (1=S1...4=S4)
		CHRG,	  1,	// 0=Battery not charging/1=Battery charging
		RI_W,	  1,	// Wake-up event source is RI
		KB_W,	  1,	// Wake-up event source is keyboard
		BATW,	  1,	// Wake-up event source is Battery Low
		PMEW,	  1,	// Wake-up event source is PME

		// Battery 0 Registers
		// Battery Life = BRC0/BPR0
		// Battery percentage = BRC0/BFC0 * 100
		BDC0,	 16,	// Designed Capacity
		BFC0,	 16,	// Last Full Charge Capacity
		BDV0,	 16,	// Design Voltage
		BPR0,	 16,	// Battery Present Rate
		BRC0,	 16,	// Battery Remaining Capacity
		BPV0,	 16,	// Battery Present Voltage

		Offset(0x11),
		CTMP,	  8,	// CPU Temperature
		Offset(0x15),
		CTRO,	  8,	// EC throttling on trip point
		CRTT,	  8,	// Critical Shut-down Temperature
		Offset(0x17),
		BKLL,	  8,	// Backlight Level

		// Battery 2 Registers
		Offset(0x20),
		,	  4,
		BTY2,	  1,	// Battery Type (0=LiIon/1=NiMh)
		,	  2,
		BAT2,	  1,	// Battery (0=Not Present/1=Present)
		,	  3,
		CRG2,	  1,	// 0=Battery not charging/1=Battery charging
		Offset(0x22),
		BDC2,	 16,	// Designed Capacity
		BFC2,	 16,	// Last Full Charge Capacity
		BDV2,	 16,	// Design Voltage
		BPR2,	 16,	// Battery Present Rate
		BRC2,	 16,	// Battery Remaining Capacity
		BPV2,	 16,	// Battery Present Voltage
		BTP2,	 16,	// Trip Point

		PBMO,	  2,	// PBMO power control method
		ECO,	  1,	// ECO on/off status
		SUN,	  1,	// Sunlight Readable
		RF,	  1,	// RF Enable/Disable status
		DOCK,	  1,	// Dock In/Out status
		CBAT,	  1,	// Car Battery status
		APPS,	  1,	// OSD utility status
		Offset(0x33),
		ODDS,	  1
	}

	Method (_CRS, 0, Serialized)
	{
		Name (ECMD, ResourceTemplate()
		{
			IO (Decode16, 0x62, 0x62, 1, 1)
			IO (Decode16, 0x66, 0x66, 1, 1)
		})

		Return (ECMD)
	}

	// EC Query methods, called upon SCI interrupts.
	Method (_Q01, 0)
	{
		Notify (\_SB.CP00, 0x80)
		If (ADP) {
			\_SB.AC.ACST = 1
			TRAP (0xe3)
			PWRS = 1
			TRAP (0x2b)
		} Else {
			\_SB.AC.ACST = 0
			Notify (\_SB.AC, 0x80)
			Notify (\_SB.BAT0, 0x80)
			PWRS = 0
			TRAP(0x2b)
		}

		PNOT()
		Notify(\_SB.BAT0, 0x80) // Execute BAT0 _BST
		Notify(\_SB.BAT1, 0x80) // Execute BAT1 _BST
	}

	Method (_Q02, 0)
	{
		If (BAT) {
			Notify(\_SB.BAT0, 0x00)
			Notify(\_SB.AC, 0x80)
		} Else {
			Notify(\_SB.AC, 0x80)
			Notify(\_SB.BAT0, 0x01)
		}

		PNOT()
		Notify(\_SB.BAT0, 0x80) // Execute BAT0 _BST
		Notify(\_SB.BAT1, 0x80) // Execute BAT1 _BST
	}

	Method (_Q05, 0)
	{
		Notify(SLPB, 0x80)
		PNOT()
		Notify(\_SB.BAT0, 0x80) // Execute BAT0 _BST
		Notify(\_SB.BAT1, 0x80) // Execute BAT1 _BST
	}

	Method (_Q07, 0)
	{
		TRAP(0xe0)

		If (RTCF == 0x00) {
			Notify(LID0, 0x80)
		} else {
			TRAP(0xc1)
		}
	}

	Method (_Q09, 0)
	{
		Notify(BAT0, 0x80)
		Notify(BAT1, 0x80)
	}

	Method (_Q0A, 0)
	{
		Notify(\_TZ.THRM, 0x80)
	}

	Method (_Q20, 0)
	{
		Notify(\_SB.ECO, 0x81)
	}

	Method (_Q21, 0)
	{
		Notify(\_SB.ECO, 0x82)
	}

	Method (_Q22, 0)
	{
		Notify(\_SB.ECO, 0x83)
	}

	Method (_Q23, 0)
	{
		Notify(\_SB.ECO, 0x84)
	}

	Method (_Q24, 0)
	{
		HOTK = 0x3f
		If (IGDS) {
			Notify (\_SB.PCI0.GFX0, 0x82)
		} Else {
			TRAP (0xE1)
		}
		Notify (\_SB.ECO, 0x85)
	}

	Method (_Q25, 0)
	{
		HOTK = 0x40
		TRAP(0xe1)
		Notify(\_SB.ECO, 0x86)
	}

	Method (_Q26, 0)
	{
		HOTK = 0x41
		TRAP(0xe1)
		Notify(\_SB.ECO, 0x87)
	}

	Method (_Q27, 0)
	{
		Notify(\_SB.ECO, 0x88)
	}

	Method (_Q28, 0)
	{
		Notify(\_SB.ECO, 0x89)
	}

	Method (_Q29, 0)
	{
		Notify(\_SB.ECO, 0x8a)
	}

	Method (_Q2A, 0)
	{
		HOTK = 0x57
		TRAP(0xe1)
		Notify(\_SB.ECO, 0x8b)
	}

	Method (_Q2B, 0)
	{
		Notify(SLPB, 0x80)
		Notify(\_SB.ECO, 0x8c)
	}

	Method (_Q2C, 0)
	{
		HOTK = 0x59
		TRAP(0xe1)
	}

	Method (_Q38, 0)
	{
		// IDE TODO
	}

	Method (_Q39, 0)
	{
		// SATA TODO
	}

	Method (_Q3A, 0)
	{
		BRTL = 1
		Notify(\_SB.ECO, 0x93)
	}

	Method (_Q3B, 0)
	{
		BRTL = 0
		Notify(\_SB.ECO, 0x93)
	}

	Method (_Q3C, 0)
	{
		SUN = 1
		Notify(\_SB.ECO, 0x92)
	}

	Method (_Q3D, 0)
	{
		SUN = 0
		Notify(\_SB.ECO, 0x92)
	}

	/* Enable RF device */
	Method (_Q3E, 0)
	{
		TRAP(0xdf)
	}

	/* Disable RF device */
	Method (_Q3F, 0)
	{
		TRAP(0xc0)
	}

	/* ACBS LAN Power On */
	Method (_Q40, 0)
	{
		TRAP(0xd0)
		Sleep(500)
		Notify(RP04, 0)
	}

	/* ACBS LAN Power Off */
	Method (_Q41, 0)
	{
		TRAP(0xd1)
	}

	Method (_Q42, 0)
	{
		TRAP(0xf3)
	}

	Method (_Q43, 0)
	{
		TRAP(0xf4)
	}

	Method (_Q48, 0)
	{
		TRAP(0xd2) // Check AC Status
		ODDS = 1
		Notify(\_SB.ECO, 0x90)
	}

	Method (_Q49, 0)
	{
		TRAP(0xd2) // Check AC Status
		ODDS = 0
		Notify(\_SB.ECO, 0x90)
	}


	Method (_Q4C, 0)
	{
		Notify(\_SB.ECO, 0x94)
	}

	Method (_Q4D, 0)
	{
		Notify(\_SB.ECO, 0x95)
	}

	Method (_Q4E, 0)
	{
		// PATA TODO
	}

	Method (_Q4F, 0)
	{
		TRAP(0xf9)
		Notify(\_SB.ECO, 0x95)
	}

	Method (_Q5C, 0)
	{
		// IGPS = 2
		Notify(\_SB.ECO, 0x94)
	}

	Method (_Q70, 0)
	{
		Notify(\_SB.ECO, 0x96)
	}

	Method (_Q71, 0)
	{
		Notify(\_SB.ECO, 0x97)
	}

	// TODO Scope _SB devices for AC power, LID, Power button
}

Scope(\_SB)
{
	/* This device is used by the GETAC P470 Windows drivers. */

	Device (ECO)
	{
		Name(_HID, "MTC0303") // MTC0303 BIOS Service Provider
		Method (GDPD, 0, Serialized)
		{
			// Set flag byte to zero
			Local0 = 0

			If (BRTL & 0x01) {
				Local0 |= 0x01
			}

			If (BRTL & 0x02) {
				Local0 |= 0x04
			}

			If (BRTL & 0x04) {
				Local0 |= 0x02
			}

			If (BRTL & 0x30) {
				Local0 |= 0x10
			}

			If (BRTL & 0x40) {
				Local0 |= 0x40
			}

			Return (Local0)
		}

		Method (GDPC, 0, Serialized)
		{
			Local0 = 0

			If (BRTL & 0x10) {
				Local0 |= 0x04
			}

			If (BRTL & 0x20) {
				Local0 |= 0x01
			}

			If (BRTL & 0x40) {
				Local0 |= 0x02
			}

			Return (Local0)
		}

		/* Set Brightness Level */
		Method(SBLL, 1, Serialized)
		{
			BRTL = Arg0
			TRAP(0xd5)	// See mainboard's smihandler.c
			Return (0)
		}

		/* Get Brightness Level */
		Method(GBLL, 0, Serialized)
		{
			TRAP(0xd6)	// See mainboard's smihandler.c
			Return (BRTL)
		}

		/* Get Brightness Level Medium? */
		Method(GBLM, 0, Serialized)
		{
			BRTL = 0x3f
			// XXX don't we have to set the brightness?
			Return(BRTL)
		}

		/* ??? */
		Method(SUTE, 1, Serialized)
		{
			If (Arg0 & 0x01) {
				TRAP(0xf5)
			} Else {
				TRAP(0xf6)
			}
			Return (0)
		}

		/* ??? */
		Method(GECO, 0, Serialized)
		{
			TRAP(0xd7)
			Return (ODDS)
		}

		/* ??? */
		Method(GBSL, 0, Serialized)
		{
			TRAP(0xd8)
			Return (BRTL)
		}

		/* ??? Get LED/Device Enable Status */
		Method(GRFD, 0, Serialized)
		{
			/* Let coreboot update the flags */
			TRAP(0xe5)

			Local0 = 0
			If (RFDV & 0x01) {
				Local0 |= 0x01
			}
			If (RFDV & 0x02) {
				Local0 |= 0x02
			}
			If (RFDV & 0x04) {
				Local0 |= 0x04
			}
			If (RFDV & 0x08) {
				Local0 |= 0x08
			}
			If (GP15 & 0x01) {		// GDIS
				Local0 |= 0x10
			}
			If (GP12 & 0x01) {		// WIFI Led (WLED)
				Local0 |= 0x20
			}
			If (BTEN & 0x01) {		// BlueTooth Enable
				Local0 |= 0x40
			}
			If (GP10 & 0x01) {		// GPS Enable
				Local0 |= 0x80
			}

			Return (Local0)
		}

		/* Set RFD */
		Method(SRFD, 1, Serialized)
		{
			If (Arg0 & 0x01) {
				GP14 = 1		// GLED
				GP15 = 1		// GDIS
			} Else {
				GP14 = 0
				GP15 = 0
			}

			/* WIFI */
			If (Arg0 & 0x02) {
				GP12 = 1		// WLED
				GP25 = 1		// WLAN
			} Else {
				GP12 = 0
				GP25 = 0
			}

			/* Bluetooth */
			If (Arg0 & 0x04) {
				GP13 = 1		// BLED
				BTEN = 1
			} Else {
				GP13 = 0		// BLED
				BTEN = 0
			}
			Return (0)
		}

		/* Get DKD */
		Method(GDKD, 0, Serialized)
		{
			TRAP(0xd9)
			Return (BRTL)
		}

		/* Set DKD */
		Method(SDKD, 0, Serialized)
		{
			TRAP(0xda)
			Return (0)
		}

		/* Set IGD (Graphics) */
		Method(SIGD, 1, Serialized)
		{
			If (Arg0 & 0x01) {
				TRAP(0xf7)
			} Else {
				TRAP(0xf8)
			}
			Return (0)
		}

		/* SMI-C? Set Mic? */
		Method (SMIC, 1, Serialized)
		{
			If (Arg0 & 0x01) {
				TRAP(0xeb)
			} Else {
				TRAP(0xec)
			}
			Return (0)
		}

		/* ??? */
		Method(GTSD, 0, Serialized)
		{
			Return (GP19)	// TSDT
		}

		/* Not even decent function names anymore? */
		Method(S024, 1, Serialized)
		{
			If (Arg0 & 0x01) {
				TRAP(0xf1)
			} Else {
				TRAP(0xf2)
			}
			Return (0)
		}

		/* Get CVD */
		Method(GCVD, 0, Serialized)
		{
			TRAP(0xf9)
			Return (BRTL)
		}

		/* ??? Something with PATA */
		Method(S025, 1, Serialized)
		{
			If (Arg0 & 0x01) {
				TRAP(0xfc)

				GP33 = 1		// CREN
				Sleep(1500)

				GP34 = 1		// CRRS
				Sleep(500)

				Notify(^^PCI0.PATA, 0)
				Notify(^^PCI0.PATA.PRID, 0)
			} Else {
				TRAP(0xfb)
				Sleep(1500)
				GP33 = 0		// CREN
				Sleep(1500)
				Notify(^^PCI0.PATA, 0)
				Notify(^^PCI0.PATA.PRID, 0)
				Notify(^^PCI0.PATA.PRID.DSK1, 1)
				Notify(^^PCI0.PATA.PRID.DSK0, 1)
			}
			Return (0)
		}

		/* ??? */
		Method(G021, 0, Serialized)
		{
			TRAP(0xfe)
			If (ACIN == 0) {
				TRAP(0xfa)
				TRAP(0xfd)
				If (ODDS == 1) {
					TRAP(0xfb)
					Notify(^^PCI0.PATA, 0)
					Notify(^^PCI0.PATA.PRID.DSK1, 1)
					Notify(^^PCI0.PATA.PRID.DSK0, 1)
					Sleep (1500)
					GP33 = 0		// CREN
					Sleep (1500)
					Notify(^^PCI0.PATA, 0)
					Notify(^^PCI0.PATA.PRID.DSK1, 1)
					Notify(^^PCI0.PATA.PRID.DSK0, 1)
					Return (0)
				}
			}
			/* All Else Cases */
			Notify(ECO, 0xb1)
			Return (1)
		}

		/* Get RFS */
		Method(GRFS, 0, Serialized)
		{
			TRAP(0xff)
			Return(BRTL)
		}

		/* ??? */
		Method(S00B, 1, Serialized)
		{
			If (Arg0 & 1) {
				TRAP(0xdc)
			} Else {
				TRAP(0xdd)
			}
			Return (0)
		}



	}
}
