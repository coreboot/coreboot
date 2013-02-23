/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Christoph Grenz <christophg+cb@grenz-bonn.de>
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

/*
 * include this file into a mainboards DSDT inside the PCI device
 * "K8 Miscellaneous Control" and it will expose the temperature
 * sensors of the processor as thermal zones.
 *
 * If, for example, the K8 Misc. Control device is on 0:18.3, include the
 * following inside the PCI0 device:
 *
 * Device(K8MC) {
 *   Name (_ADR, 0x00180003)
 *   #include northbridge/amd/amdk8/thermal_mixin.asl
 * }
 *
 * Note: as only the current temperature and the trip temperature for
 * "Software Thermal Control" are available in the PCI registers, but the
 * linux driver for thermal zones needs a critical temperature value, a
 * reasonable critical temperature is calculated by simply adding 6°C to
 * the trip temperature.
 *
 * The used registers are documented in the "BIOS and Kernel Developer's
 * Guide for AMD NPT Family 0Fh Processors"
 * http://support.amd.com/us/Processor_TechDocs/32559.pdf
 *
 */

#ifndef K8TEMP_CRITICAL_ADD
# define K8TEMP_CRITICAL_ADD 6
#endif

OperationRegion(K8TR, PCI_Config, 0xE4, 0x4)
Field(K8TR, DWordAcc, NoLock, Preserve) {
	,     1,
	THTP, 1, /* Temperature sensor trip occured */
	CORE, 1, /* Select Core */
	TTS0, 1, /* Temperature sensor trip on CPU1 (or single core CPU0) */
	TTS1, 1, /* Temperature sensor trip on CPU0 */
	TTEN, 1, /* Temperature sensor trip enabled */
	PLAC, 1, /* Select Sensor */
	,     1,
	DOFF, 6, /* Diode offset (signed 6bit-Integer) in °C */
	TFRC, 2, /* Temperature fractions */
	TVAL, 8, /* Temperature value in °C biased by -49 */
	TJOF, 5,
	,     2,
	SWTT, 1, /* Induce a thermtrip event (for diagnostic purposes) */
}

OperationRegion(K8ST, PCI_Config, 0x70, 0x1)
Field(K8ST, ByteAcc, NoLock, Preserve) {
	TMAX, 5, /* Maximum temperature for software thermal control, in °C, biased by 52 */
}

/* Calculates temperature in tenths Kelvin from given TVAL and TFRC values */
Method(K8PT, 2) {
	Divide(Multiply(Arg1, 5), 2, , Local0)
	Return (Add(Multiply(Add(Arg0, 224),10), Local0))
}

/* Calculates the diode offset from a DOFF value */
Method(K8PO, 1) {
	If (And(Arg0, 0x20))
	{
		Return (Multiply(Subtract(Xor(Arg0, 0x3F), 1), 10))
	}
	Else {
		Return (Multiply(Arg0, 10))
	}
}

ThermalZone (K8T0) {
	Name(_HID, EisaId("PNP0C11"))
	Name(_UID, "k8-0")
	Name(_STR, Unicode("K8 compatible CPU Core 1 Thermal Sensor 1"))

	Method(_STA) {
		Store(CORE, Local0)
		Store(PLAC, Local1)

		Store(Zero, CORE)
		Store(Zero, PLAC)
		If (LOr(PLAC, CORE)) {
			Store(Local0, CORE)
			Store(Local1, PLAC)
			Return (0x00)
		}

		If (LOr(LNot(TVAL), LEqual(TVAL, 0xFF))) {
			Return (0x00)
		}

		Store(Local0, CORE)
		Store(Local1, PLAC)
		Return (0x0F)
	}

	Method(_TMP) {
		Store(CORE, Local0)
		Store(PLAC, Local1)

		Store(Zero, CORE)
		Store(Zero, PLAC)

		Store (K8PT(TVAL, TFRC), Local2)
		Add (K8PO(DOFF), Local2, Local2)
		Store(Local0, CORE)
		Store(Local1, PLAC)

		Return (Local2)
	}

	Method(_CRT) {
		Add(TMAX, 325, Local0)
		Add(Local0, K8TEMP_CRITICAL_ADD, Local0)
		Return (Multiply(Local0, 10))
	}
}

ThermalZone (K8T1) {
	Name(_HID, EisaId("PNP0C11"))
	Name(_UID, "k8-1")
	Name(_STR, Unicode("K8 compatible CPU Core 1 Thermal Sensor 2"))

	Name(_TZD, Package () {\_PR.CPU0})

	Method(_STA) {
		Store(CORE, Local0)
		Store(PLAC, Local1)

		Store(Zero, CORE)
		Store(Zero, PLAC)
		If (LOr(PLAC, CORE)) {
			Store(Local0, CORE)
			Store(Local1, PLAC)
			Return (0x00)
		}

		Store(One, PLAC)
		If (LOr(LNot(TVAL), LEqual(TVAL, 0xFF))) {
			Return (0x00)
		}

		Store(Local0, CORE)
		Store(Local1, PLAC)
		Return (0x0F)
	}

	Method(_TMP) {
		Store(CORE, Local0)
		Store(PLAC, Local1)

		Store(Zero, CORE)
		Store(One, PLAC)

		Store (K8PT(TVAL, TFRC), Local2)
		Add (K8PO(DOFF), Local2, Local2)
		Store(Local0, CORE)
		Store(Local1, PLAC)

		Return (Local2)
	}

	Method(_CRT) {
		Add(TMAX, 325, Local0)
		Add(Local0, K8TEMP_CRITICAL_ADD, Local0)
		Return (Multiply(Local0, 10))
	}
}

ThermalZone (K8T2) {
	Name(_HID, EisaId("PNP0C11"))
	Name(_UID, "k8-2")
	Name(_STR, Unicode("K8 compatible CPU Core 2 Thermal Sensor 1"))

	Name(_TZD, Package () {\_PR.CPU0})

	Method(_STA) {
		Store(CORE, Local0)
		Store(PLAC, Local1)

		Store(Zero, CORE)
		Store(Zero, PLAC)
		If (LOr(PLAC, CORE)) {
			Store(Local0, CORE)
			Store(Local1, PLAC)
			Return (0x00)
		}

		Store(One, CORE)
		If (LOr(LNot(TVAL), LEqual(TVAL, 0xFF))) {
			Return (0x00)
		}

		Store(Local0, CORE)
		Store(Local1, PLAC)
		Return (0x0F)
	}

	Method(_TMP) {
		Store(CORE, Local0)
		Store(PLAC, Local1)

		Store(One, CORE)
		Store(Zero, PLAC)

		Store (K8PT(TVAL, TFRC), Local2)
		Add (K8PO(DOFF), Local2, Local2)
		Store(Local0, CORE)
		Store(Local1, PLAC)

		Return (Local2)
	}

	Method(_CRT) {
		Add(TMAX, 325, Local0)
		Add(Local0, K8TEMP_CRITICAL_ADD, Local0)
		Return (Multiply(Local0, 10))
	}
}

ThermalZone (K8T3) {
	Name(_HID, EisaId("PNP0C11"))
	Name(_UID, "k8-3")
	Name(_STR, Unicode("K8 compatible CPU Core 2 Thermal Sensor 2"))

	Name(_TZD, Package () {\_PR.CPU0})

	Method(_STA) {
		Store(CORE, Local0)
		Store(PLAC, Local1)

		Store(Zero, CORE)
		Store(Zero, PLAC)
		If (LOr(PLAC, CORE)) {
			Store(Local0, CORE)
			Store(Local1, PLAC)
			Return (0x00)
		}

		Store(One, CORE)
		Store(One, PLAC)
		If (LOr(LNot(TVAL), LEqual(TVAL, 0xFF))) {
			Return (0x00)
		}

		Store(Local0, CORE)
		Store(Local1, PLAC)
		Return (0x0F)
	}

	Method(_TMP) {
		Store(CORE, Local0)
		Store(PLAC, Local1)

		Store(One, CORE)
		Store(One, PLAC)

		Store (K8PT(TVAL, TFRC), Local2)
		Add (K8PO(DOFF), Local2, Local2)
		Store(Local0, CORE)
		Store(Local1, PLAC)

		Return (Local2)
	}

	Method(_CRT) {
		Add(TMAX, 325, Local0)
		Add(Local0, K8TEMP_CRITICAL_ADD, Local0)
		Return (Multiply(Local0, 10))
	}
}