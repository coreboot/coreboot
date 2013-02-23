/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Tobias Diedrich <ranma+coreboot@tdiedrich.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

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
		Store(0x80, pinx)			\
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
			Decrement(Local0)		\
		}					\
		Store(Local0, pinx)			\
	}						\
}							\

PCI_INTX_DEV(LNKA, PRTA, 1)
PCI_INTX_DEV(LNKB, PRTB, 2)
PCI_INTX_DEV(LNKC, PRTC, 3)
PCI_INTX_DEV(LNKD, PRTD, 4)
