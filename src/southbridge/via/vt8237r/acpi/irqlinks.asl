/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Lubomir Rintel <lkundrak@v3.sk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/x86/acpi/statdef.asl>

/* Possible PNP IRQs */
Name (PIRQ, ResourceTemplate () {
	IRQ (Level, ActiveLow, Shared) {3, 4, 6, 7, 10, 11, 12}
})

#define IRQ_LINK(reg)						\
	Name (_HID, EisaId ("PNP0C0F"))				\
	Name (_UID, 1)						\
								\
	Method (_STA, 0) {					\
		If (LEqual (reg, 0x00)) {			\
			Return (STA_DISABLED)			\
		}						\
		Return (STA_INVISIBLE)				\
	}							\
								\
	Method (_PRS, 0) {					\
		Return (PIRQ)					\
	}							\
								\
	Name (CRSA, ResourceTemplate () {			\
		IRQ (Level, ActiveLow, Shared) {}		\
	})							\
	Method (_CRS, 0) {					\
		CreateWordField (CRSA, 0x1, AINT)		\
		ShiftLeft (One, reg, AINT)			\
		Return (CRSA)					\
	}							\
								\
	Method (_SRS, 1) {					\
		CreateWordField (Arg0, 0x1, AINT)		\
		Store (Zero, Local0)				\
		Store (AINT, Local1)				\
		While (LNotEqual (Local1, One)) {		\
			ShiftRight (Local1, One, Local1)	\
			Increment (Local0)			\
		}						\
		Store (Local0, reg)				\
	}							\
								\
	Method (_DIS, 0) {					\
		Store (Zero, reg)				\
	}							\

Device (LNKA) { IRQ_LINK (INTA) }
Device (LNKB) { IRQ_LINK (INTB) }
Device (LNKC) { IRQ_LINK (INTC) }
Device (LNKD) { IRQ_LINK (INTD) }
