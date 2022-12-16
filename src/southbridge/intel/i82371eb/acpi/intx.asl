/* SPDX-License-Identifier: GPL-2.0-only */
Name(IRQB, ResourceTemplate(){
	IRQ(Level,ActiveLow,Shared){}
})

Name(IRQP, ResourceTemplate(){
	IRQ(Level,ActiveLow,Shared){3, 4, 5, 6, 7, 10, 11, 12, 14, 15}
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
		IRQN = 1 << And(pinx, 0x0f)		\
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
}
