/* SPDX-License-Identifier: GPL-2.0-only */

Device(EC0)
{
	Name (_HID, EISAID("PNP0C09"))
	Name (_UID, 1)

	Name (_GPE, 23)	// GPI07 / GPE23 -> Runtime SCI
	Name (ECON, 0)
	Name (QEVT, 0)

	OperationRegion(ERAM, EmbeddedControl, 0x00, 0xff)
	Field (ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(0x04), // Command interface
		CMCM,	  8,
		CMD1,	  8,
		CMD2,	  8,
		CMD3,	  8,
		Offset(0x18), // SMBus
		SMPR,	  8,
		SMST,	  8,
		SMAD,	  8,
		SMCM,	  8,
		Offset(0x1c),
		SMW0,	 16,
		//SMD0,	264,
		Offset(0x3d),
		SMAA,	  8,
		Offset(0x78), // GPIs
		P60S,	  1,
		P61S,	  1,
		P62S,	  1,
		P63S,	  1,
		P64S,	  1,
		P65S,	  1,
		P66S,	  1,
		P67S,	  1,
		Offset(0x83), // Thermal
		RTMP,	  8,
		TML0,	  8,
		TMH0,	  8,
		Offset(0x87),
		TMCR,	  8,
		Offset(0x89),
		TML1,	  8,
		TMH1,	  8
	}

	Method (_CRS, 0)
	{
		Name (ECMD, ResourceTemplate()
		{
			IO (Decode16, 0x62, 0x62, 1, 1)
			IO (Decode16, 0x66, 0x66, 1, 1)
		})

		Return (ECMD)
	}

	Method (_REG, 2)
	{
		// This method is needed by Windows XP/2000 for
		// EC initialization before a driver is loaded

		If (Arg0 == 0x03) {
			ECON = Arg1
		}
	}

	// EC Query methods

	Method (_Q11, 0)
	{
		Debug = "_Q11: Fn-F8 (Sleep Button) pressed"
		Notify(SLPB, 0x80)
	}

	Method (_Q12, 0)
	{
		Debug = "_Q12: Fn-F9 (Display Switch) pressed"
		Notify (\_SB.PCI0.GFX0, 0x82)
		// TLST = 1
	}

	Method (_Q30, 0)
	{
		Debug = "_Q30: AC In/Out"
		Notify(ADP1, 0x80)	// Tell the Power Adapter
		PNOT()			// and the CPU and Battery
		// Notify the Batteries
		Notify(BAT1, 0x80) // Execute BAT1 _BST
		Notify(BAT2, 0x80) // Execute BAT2 _BST
	}

	Method (_Q31, 0)
	{
		Debug = "_Q31: LID Open/Close"
		Notify(LID0, 0x80)
	}

	Method (_Q32, 0)
	{
		Debug = "_Q32: Battery 1 In/Out"
		If (ECON) {
			Local0 = P62S
			If (~Local0) {
				Notify(BAT1, 0x80)
			}
		}
	}

	Method (_Q33, 0)
	{
		Debug = "_Q33: Battery 2 In/Out"
		If (ECON) {
			Local0 = P63S
			If (~Local0) {
				Notify(BAT2, 0x80)
			}
		}
	}

	Method (_Q34, 0)
	{
		Debug = "_Q34: LPT/FDD"
		// PHSS(0x70)
	}

	Method (_Q35, 0)
	{
		Debug = "_Q35: Processor is hot"
	}

	Method (_Q36, 0)
	{
		Debug = "_Q36: Thermal Warning"
	}

	Method (_Q37, 0)
	{
		Debug = "_Q37: PME"
	}

	Method (_Q38, 0)
	{
		Debug = "_Q38: Thermal"
	}

	Method (_Q39, 0)
	{
		Debug = "_Q39: Thermal"
	}

	// TODO Scope _SB devices for AC power, LID, Power button

}
