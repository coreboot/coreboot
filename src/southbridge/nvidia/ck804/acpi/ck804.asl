/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Device (LPCB) {
	Name (_ADR, 0x00010000)
	OperationRegion (LPC0, PCI_Config, 0x00, 0x100)
	Field (LPC0, AnyAcc, NoLock, Preserve) {
		Offset (0x7c),
		INTA, 4, INTB, 4,
		INTC, 4, INTD, 4,
		INTE, 4, INTF, 4,
		INTG, 4, INTH, 4,

		INTI, 4, INTJ, 4,
		INTK, 4, INTL, 4,
		INTM, 4, INTN, 4,
		INTO, 4, INTP, 4,

		INTQ, 4, INTR, 4,
		INTS, 4, INTT, 4,
		INTU, 4, INTV, 4,
		INTW, 4, INTX, 4,
	}
}

/* set "A", 8259 interrupts */
Name (PRSA, ResourceTemplate () {
	IRQ(Level, ActiveLow, Shared) {5, 7, 10, 11}
})
Method (CRSA, 1, Serialized) {
	Name (LRTL, ResourceTemplate() {
		IRQ(Level, ActiveLow, Shared) {}
	})
	CreateWordField(LRTL, 1, LIRQ)
	ShiftLeft(1, Arg0, LIRQ)
	Return (LRTL)
}
Method (SRSA, 1, Serialized) {
	CreateWordField(Arg0, 1, LIRQ)
	FindSetRightBit(LIRQ, Local0)
	Decrement(Local0)
	Return (Local0)
}

/* set "B", external (PCI) APIC interrupts */
Name (PRSB, ResourceTemplate () {
	Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,,) {
		16, 17, 18, 19,
	}
})
Method (CRSB, 1, Serialized) {
	Name (LRTL, ResourceTemplate() {
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,,) { 0 }
	})
	CreateDWordField (LRTL, 5, LIRQ)
	If (LEqual (Arg0, 8)) {
		Store (16, LIRQ)
	} ElseIf (LEqual (Arg0, 1)) {
		Store (17, LIRQ)
	} ElseIf (LEqual (Arg0, 2)) {
		Store (18, LIRQ)
	} ElseIf (LEqual (Arg0, 13)) {
		Store (19, LIRQ)
	} Else {
		Store (0, LIRQ)
	}
	Return (LRTL)
}
Method (SRSB, 1, Serialized) {
	CreateDWordField(Arg0, 5, LIRQ)
	If (LEqual (LIRQ, 16)) {
		Return (8)
	} ElseIf (LEqual (LIRQ, 17)) {
		Return (1)
	} ElseIf (LEqual (LIRQ, 18)) {
		Return (2)
	} ElseIf (LEqual (LIRQ, 19)) {
		Return (13)
	} Else {
		Return (0)
	}
}

/* set "C", southbridge APIC interrupts */
Name (PRSC, ResourceTemplate () {
	Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,,) {
		20, 21, 22, 23,
	}
})
Method (CRSC, 1, Serialized) {
	Name (LRTL, ResourceTemplate() {
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,,) { 0 }
	})
	CreateDWordField (LRTL, 5, LIRQ)
	If (LEqual (Arg0, 8)) {
		Store (20, LIRQ)
	} ElseIf (LEqual (Arg0, 13)) {
		Store (21, LIRQ)
	} ElseIf (LEqual (Arg0, 2)) {
		Store (22, LIRQ)
	} ElseIf (LEqual (Arg0, 1)) {
		Store (23, LIRQ)
	} Else {
		Store (0, LIRQ)
	}
	Return (LRTL)
}
Method (SRSC, 1, Serialized) {
	CreateDWordField(Arg0, 5, LIRQ)
	If (LEqual (LIRQ, 20)) {
		Return (8)
	} ElseIf (LEqual (LIRQ, 21)) {
		Return (13)
	} ElseIf (LEqual (LIRQ, 22)) {
		Return (2)
	} ElseIf (LEqual (LIRQ, 23)) {
		Return (1)
	} Else {
		Return (0)
	}
}

Device (LNKA) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 1)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTA) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTA)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSB)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSB(\_SB.PCI0.LPCB.INTA))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTA))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSB(Arg0), \_SB.PCI0.LPCB.INTA)
		} Else {
			Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTA)
		}
	}
}
Device (LNKB) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 2)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTB) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTB)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSB)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSB(\_SB.PCI0.LPCB.INTB))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTB))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSB(Arg0), \_SB.PCI0.LPCB.INTB)
		} Else {
			Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTB)
		}
	}
}
Device (LNKC) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 3)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTC) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTC)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSB)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSB(\_SB.PCI0.LPCB.INTC))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTC))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSB(Arg0), \_SB.PCI0.LPCB.INTC)
		} Else {
			Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTC)
		}
	}
}
Device (LNKD) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 4)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTD) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTD)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSB)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSB(\_SB.PCI0.LPCB.INTD))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTD))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSB(Arg0), \_SB.PCI0.LPCB.INTD)
		} Else {
			Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTD)
		}
	}
}
Device (LNKE) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 5)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTE) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTE)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSB)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSB(\_SB.PCI0.LPCB.INTE))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTE))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSB(Arg0), \_SB.PCI0.LPCB.INTE)
		} Else {
			Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTE)
		}
	}
}
Device (LLAS) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 6)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTK) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTK)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSC)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSC(\_SB.PCI0.LPCB.INTK))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTK))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSC(Arg0), \_SB.PCI0.LPCB.INTK)
		} Else {
			Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTK)
		}
	}
}
Device (LUOH) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 7)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTQ) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTQ)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSC)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSC(\_SB.PCI0.LPCB.INTQ))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTQ))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSC(Arg0), \_SB.PCI0.LPCB.INTQ)
		} Else {
			Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTQ)
		}
	}
}
Device (LUEH) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 8)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTL) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTL)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSC)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSC(\_SB.PCI0.LPCB.INTL))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTL))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSC(Arg0), \_SB.PCI0.LPCB.INTL)
		} Else {
			Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTL)
		}
	}
}
Device (LAUD) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 9)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTU) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTU)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSC)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSC(\_SB.PCI0.LPCB.INTU))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTU))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSC(Arg0), \_SB.PCI0.LPCB.INTU)
		} Else {
			Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTU)
		}
	}
}
Device (LMOD) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 10)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTV) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTV)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSC)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSC(\_SB.PCI0.LPCB.INTV))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTV))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSC(Arg0), \_SB.PCI0.LPCB.INTV)
		} Else {
			Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTV)
		}
	}
}
Device (LPA0) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 11)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTW) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTW)
		Store (0, \_SB.PCI0.LPCB.INTX)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSC)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSC(\_SB.PCI0.LPCB.INTW))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTW))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSC(Arg0), Local0)
		} Else {
			Store (SRSA(Arg0), Local0)
		}
		Store(Local0, \_SB.PCI0.LPCB.INTW)
		Store(Local0, \_SB.PCI0.LPCB.INTX)
	}
}
Device (LSA0) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 12)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTP) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTP)
		Store (0, \_SB.PCI0.LPCB.INTG)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSC)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSC(\_SB.PCI0.LPCB.INTP))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTP))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSC(Arg0), Local0)
		} Else {
			Store (SRSA(Arg0), Local0)
		}
		Store(Local0, \_SB.PCI0.LPCB.INTP)
		Store(Local0, \_SB.PCI0.LPCB.INTG)
	}
}
Device (LSA1) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 13)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTO) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTO)
		Store (0, \_SB.PCI0.LPCB.INTF)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSC)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSC(\_SB.PCI0.LPCB.INTO))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTO))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSC(Arg0), Local0)
		} Else {
			Store (SRSA(Arg0), Local0)
		}
		Store(Local0, \_SB.PCI0.LPCB.INTO)
		Store(Local0, \_SB.PCI0.LPCB.INTF)
	}
}
Device (LEMA) {
	Name (_HID, EISAID ("PNP0C0F"))
	Name (_UID, 14)
	Method (_STA, 0, Serialized) {
		If (\_SB.PCI0.LPCB.INTS) {
			Return (0xb)
		} Else {
			Return (0x9)
		}
	}
	Method (_DIS, 0, Serialized) {
		Store (0, \_SB.PCI0.LPCB.INTS)
	}
	Method (_PRS, 0, Serialized) {
		If (PICM) {
			Return (PRSC)
		} Else {
			Return (PRSA)
		}
	}
	Method (_CRS, 0, Serialized) {
		If (PICM) {
			Return (CRSC(\_SB.PCI0.LPCB.INTS))
		} Else {
			Return (CRSA(\_SB.PCI0.LPCB.INTS))
		}
	}
	Method (_SRS, 1, Serialized) {
		If (PICM) {
			Store (SRSC(Arg0), \_SB.PCI0.LPCB.INTS)
		} Else {
			Store (SRSA(Arg0), \_SB.PCI0.LPCB.INTS)
		}
	}
}
