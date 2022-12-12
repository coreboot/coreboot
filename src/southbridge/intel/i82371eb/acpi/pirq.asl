/* SPDX-License-Identifier: GPL-2.0-only */

Field (\_SB.PCI0.LPCB.PCIC, AnyAcc, NoLock, Preserve)
{
	Offset (0x60),	// Interrupt Routing Registers
	PRTA,	8,
	PRTB,	8,
	PRTC,	8,
	PRTD,	8,
}

Name(IRQB, ResourceTemplate(){
	IRQ(Level,ActiveLow,Shared){15}
})

Name(IRQP, ResourceTemplate(){
	IRQ(Level,ActiveLow,Exclusive){3, 4, 5, 6, 7, 10, 11, 12}
})

/* adapted from ma78gm/dsdt.asl */
#define PCI_INTX_DEV(intx, pinx, uid)			\
Device(intx) {						\
	Name(_HID, EISAID("PNP0C0F"))			\
	Name(_UID, uid)					\
							\
	Method(_STA, 0) {				\
		If (And(pinx, 0x80)) {			\
			Return(0x09)			\
		}					\
		Return(0x0B)				\
	}						\
							\
	Method(_DIS ,0) {				\
		pinx = 0x80			\
	}						\
							\
	Method(_PRS ,0) {				\
		Return(IRQP)				\
	}						\
							\
	Method(_CRS ,0) {				\
		CreateWordField(IRQB, 1, IRQN)		\
		ShiftLeft(1, And(pinx, 0x0f), IRQN)	\
		Return(IRQB)				\
	}						\
							\
	Method(_SRS, 1) {				\
		CreateWordField(ARG0, 1, IRQM)		\
							\
		/* Use lowest available IRQ */		\
		FindSetRightBit(IRQM, Local0)		\
		if (Local0) {				\
			Local0--			\
		}					\
		pinx = Local0			\
	}						\
}							\

PCI_INTX_DEV(LNKA, PRTA, 1)
PCI_INTX_DEV(LNKB, PRTB, 2)
PCI_INTX_DEV(LNKC, PRTC, 3)
PCI_INTX_DEV(LNKD, PRTD, 4)
