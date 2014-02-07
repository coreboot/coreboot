/*
 * Copyright 2005 AMD
 * Copyright (C) 2011,2014 Oskar Enoksson <enok@lysator.liu.se>
 */
//AMD8111 pic LNKA B C D

Device (LNKA) {
	Name (_HID, EisaId ("PNP0C0F"))
	Name (_UID, 0x01)
	Method (_STA, 0, NotSerialized)	{
		If (LEqual (\_SB.PCI0.PMF.PIRA, 0x00) ) { Return (0x09) } //Disabled
		Else                                    { Return (0x0B) } //Enabled
	}
	Method (_PRS, 0, Serialized)	{
		Name (BUFA, ResourceTemplate ()	{
			IRQ (Level, ActiveLow, Shared) {3,5,10,11}
		})
		Return (BUFA)
	}
	Method (_DIS, 0, NotSerialized) {
		Store (0x00, \_SB.PCI0.PMF.PIRA )
	}

	Method (_CRS, 0, Serialized) {
		Name (BUFA, ResourceTemplate ()	{
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateByteField (BUFA, 0x01, IRA1)
		CreateByteField (BUFA, 0x02, IRA2)
		Store (0x00, Local2)
		Store (0x00, Local3)
		Store (\_SB.PCI0.PMF.PIRA, Local1)
		If (LNot (LEqual (Local1, 0x00))) {  // Routing enable
			If (LGreater (Local1, 0x07)) {
				Subtract (Local1, 0x08, Local1)
				ShiftLeft (One, Local1, Local3)
			} Else {
				If (LGreater (Local1, 0x00)) {
					ShiftLeft (One, Local1, Local2)
				}
			}
			Store (Local2, IRA1)
			Store (Local3, IRA2)
		}
		Return (BUFA)
	}

	Method (_SRS, 1, NotSerialized) {
		CreateByteField (Arg0, 0x01, IRA1)
		CreateByteField (Arg0, 0x02, IRA2)
		ShiftLeft (IRA2, 0x08, Local0)
		Or (Local0, IRA1, Local0)
		Store (0x00, Local1)
		ShiftRight (Local0, 0x01, Local0)
		While (LGreater (Local0, 0x00)) {
			Increment (Local1)
			ShiftRight (Local0, 0x01, Local0)
		}
		Store(Local1,\_SB.PCI0.PMF.PIRA)
	}
}

Device (LNKB) {
	Name (_HID, EisaId ("PNP0C0F"))
	Name (_UID, 0x02)
	Method (_STA, 0, NotSerialized) {
		If (LEqual (\_SB.PCI0.PMF.PIRB, 0x00) ) { Return (0x09) } //Disabled
		Else                                    { Return (0x0B) } //Enabled
	}

	Method (_PRS, 0, Serialized) {
		Name (BUFB, ResourceTemplate () {
			IRQ (Level, ActiveLow, Shared) {3,5,10,11}
		})
		Return (BUFB)
	}

	Method (_DIS, 0, NotSerialized) {
		Store (0x00, \_SB.PCI0.PMF.PIRB )
	}

	Method (_CRS, 0, Serialized) {
		Name (BUFB, ResourceTemplate () {
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateByteField (BUFB, 0x01, IRA1)
		CreateByteField (BUFB, 0x02, IRA2)
		Store (0x00, Local2)
		Store (0x00, Local3)
		Store (\_SB.PCI0.PMF.PIRB, Local1)
		If (LNot (LEqual (Local1, 0x00))) {  // Routing enable
			If (LGreater (Local1, 0x07)) {
				Subtract (Local1, 0x08, Local1)
				ShiftLeft (One, Local1, Local3)
			} Else {
				If (LGreater (Local1, 0x00)) {
					ShiftLeft (One, Local1, Local2)
				}
			}
			Store (Local2, IRA1)
			Store (Local3, IRA2)
		}
		Return (BUFB)
	}

	Method (_SRS, 1, NotSerialized) {
		CreateByteField (Arg0, 0x01, IRA1)
		CreateByteField (Arg0, 0x02, IRA2)
		ShiftLeft (IRA2, 0x08, Local0)
		Or (Local0, IRA1, Local0)
		Store (0x00, Local1)
		ShiftRight (Local0, 0x01, Local0)
		While (LGreater (Local0, 0x00)) {
			Increment (Local1)
			ShiftRight (Local0, 0x01, Local0)
		}
		Store(Local1,\_SB.PCI0.PMF.PIRB)
	}
}

Device (LNKC) {
	Name (_HID, EisaId ("PNP0C0F"))
	Name (_UID, 0x03)
	Method (_STA, 0, NotSerialized) {
		If (LEqual (\_SB.PCI0.PMF.PIRC, 0x00) ) { Return (0x09) } //Disabled
		Else                                    { Return (0x0B) } //Enabled
	}

	Method (_PRS, 0, Serialized) {
		Name (BUFA, ResourceTemplate () {
			IRQ (Level, ActiveLow, Shared) {3,5,10,11}
		})
		Return (BUFA)
	}

	Method (_DIS, 0, NotSerialized) {
		Store (0x00, \_SB.PCI0.PMF.PIRC )
	}

	Method (_CRS, 0, Serialized) {
		Name (BUFA, ResourceTemplate () {
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateByteField (BUFA, 0x01, IRA1)
		CreateByteField (BUFA, 0x02, IRA2)
		Store (0x00, Local2)
		Store (0x00, Local3)
		Store (\_SB.PCI0.PMF.PIRC, Local1)
		If (LNot (LEqual (Local1, 0x00))) {  // Routing enable
			If (LGreater (Local1, 0x07)) {
				Subtract (Local1, 0x08, Local1)
				ShiftLeft (One, Local1, Local3)
			} Else {
				If (LGreater (Local1, 0x00)) {
					ShiftLeft (One, Local1, Local2)
				}
			}
			Store (Local2, IRA1)
			Store (Local3, IRA2)
		}
		Return (BUFA)
	}

	Method (_SRS, 1, NotSerialized) {
		CreateByteField (Arg0, 0x01, IRA1)
		CreateByteField (Arg0, 0x02, IRA2)
		ShiftLeft (IRA2, 0x08, Local0)
		Or (Local0, IRA1, Local0)
		Store (0x00, Local1)
		ShiftRight (Local0, 0x01, Local0)
		While (LGreater (Local0, 0x00)) {
			Increment (Local1)
			ShiftRight (Local0, 0x01, Local0)
		}
		Store(Local1,\_SB.PCI0.PMF.PIRC)
	}
}

Device (LNKD) {
	Name (_HID, EisaId ("PNP0C0F"))
	Name (_UID, 0x04)
	Method (_STA, 0, NotSerialized) {
		If (LEqual (\_SB.PCI0.PMF.PIRD, 0x00) ) { Return (0x09) } //Disabled
		Else                                    { Return (0x0B) } //Enabled
	}
	Method (_PRS, 0, Serialized) {
		Name (BUFB, ResourceTemplate () {
			IRQ (Level, ActiveLow, Shared) {3,5,10,11}
		})
		Return (BUFB)
	}
	Method (_DIS, 0, NotSerialized) {
		Store (0x00, \_SB.PCI0.PMF.PIRD )
	}
	Method (_CRS, 0, Serialized) {
		Name (BUFB, ResourceTemplate () {
			IRQ (Level, ActiveLow, Shared) {}
		})
		CreateByteField (BUFB, 0x01, IRA1)
		CreateByteField (BUFB, 0x02, IRA2)
		Store (0x00, Local2)
		Store (0x00, Local3)
		Store (\_SB.PCI0.PMF.PIRD, Local1)
		If (LNot (LEqual (Local1, 0x00))) {  // Routing enable
			If (LGreater (Local1, 0x07)) {
				Subtract (Local1, 0x08, Local1)
				ShiftLeft (One, Local1, Local3)
			} Else {
				If (LGreater (Local1, 0x00)) {
					ShiftLeft (One, Local1, Local2)
				}
			}
			Store (Local2, IRA1)
			Store (Local3, IRA2)
		}
		Return (BUFB)
	}
	Method (_SRS, 1, NotSerialized) {
		CreateByteField (Arg0, 0x01, IRA1)
		CreateByteField (Arg0, 0x02, IRA2)
		ShiftLeft (IRA2, 0x08, Local0)
		Or (Local0, IRA1, Local0)
		Store (0x00, Local1)
		ShiftRight (Local0, 0x01, Local0)
		While (LGreater (Local0, 0x00)) {
			Increment (Local1)
			ShiftRight (Local0, 0x01, Local0)
		}
		Store(Local1,\_SB.PCI0.PMF.PIRD)
	}
}
