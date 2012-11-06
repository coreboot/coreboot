/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2012 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

Name(\CBA1, 0x60)
Name(\CBA2, 0x60)

/* ACPI doesn't care if capacity is given in mWh or mAh,
   but it has to be consistent */

Device (BAT1)
{
	Name(_HID, EisaId("PNP0C0A"))
	Name(_UID, 1)

	Name(_PCL, Package(){ _SB })
	Name(PSTA, 0x1f)

	/* battery information */
	Name(PBIF, Package() {
		0x01,	// power unit
		0x01,	// design capacity (mWh/mAh), updated by UPBI
		0x01,	// last full charge capacity (mWh/mAh), updated by UPBI
		0x01,	// battery technology: "rechargeable"
		0x01,	// design voltage (mV), updated by UPBI
		0x0294,	// design capacity of warning (mWh/mAh)
		0x014a, // design capacity of low (mWh/mAh)
		0x42,	// battery capacity granularity between low and warning
		0x42,	// battery capacity granularity between warning and full
		"BAT1",	// Name
		"001 ",	// Number
		"LION",	// Type
		"JSE"	// Vendor
	})
	/* battery status */
	/* dummy values, filled in by UPBS */
	Name(PBST, Package() {
		0x00,	// battery state
			// bit0: discharging
			// bit1: charging
			// bit2: critical energy state
		0x01,	// battery present rate (mW/mA)
		0x01,	// remaining capacity (mW/mA)
		0x01	// battery present voltage (mV)
	})

	/* Status */
	Method(_STA, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			If(\_SB.PCI0.LPCB.EC0.B1PR) {
				If(\_SB.PCI0.LPCB.EC0.B1CH) {
					Return(0x1f)
				}
			}
		}
		Return(0x0f)
	}

	/* Battery Info */
	Method(_BIF, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			Store (\_SB.PCI0.LPCB.EC0.B1DW, Index(PBIF, 1))
			Store (\_SB.PCI0.LPCB.EC0.B1FW, Index(PBIF, 2))
			Store (\_SB.PCI0.LPCB.EC0.B1DV, Index(PBIF, 4))
		}

		Return(PBIF)
	}

	/* Battery Status */
	Method(_BST, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			Store (\_SB.PCI0.LPCB.EC0.B1PW, Local0)
			If (LGreaterEqual (Local0, 0x8000)) {
				Subtract (0x10000, Local0, Local0)
			}
			Store (Local0, Index(PBST, 1))
			Store (\_SB.PCI0.LPCB.EC0.B1PV, Index(PBST, 3))

			Store (\_SB.PCI0.LPCB.EC0.B1RW, Index(PBST, 2))

			If (\_SB.PCI0.LPCB.EC0.ACCH) {
				If (\_SB.PCI0.LPCB.EC0.B1CH) {
					If (\_SB.PCI0.LPCB.EC0.B1CG) {
						Store (2, Index(PBST, 0))
					}
				}
			} Else {
				If (\_SB.PCI0.LPCB.EC0.B1LO) {
					Store (5, Index(PBST, 0))
				} Else {
					Store (1, Index(PBST, 0))
				}
			}
		}

		Return(PBST)
	}

}

Device (BAT2)
{
	Name(_HID, EisaId("PNP0C0A"))
	Name(_UID, 2)

	Name(_PCL, Package(){ _SB })
	Name(PSTA, 0x1f)

	/* battery information */
	Name(PBIF, Package() {
		0x01,	// power unit
		0x01,	// design capacity (mWh/mAh), updated by UPBI
		0x01,	// last full charge capacity (mWh/mAh), updated by UPBI
		0x01,	// battery technology: "rechargeable"
		0x01,	// design voltage (mV), updated by UPBI
		0x0294,	// design capacity of warning (mWh/mAh)
		0x014a, // design capacity of low (mWh/mAh)
		0x42,	// battery capacity granularity between low and warning
		0x42,	// battery capacity granularity between warning and full
		"BAT1",	// Name
		"002 ",	// Number
		"LION",	// Type
		"JSE"	// Vendor
	})
	/* battery status */
	/* dummy values, filled in by UPBS */
	Name(PBST, Package() {
		0x00,	// battery state
			// bit0: discharging
			// bit1: charging
			// bit2: critical energy state
		0x01,	// battery present rate (mW/mA)
		0x01,	// remaining capacity (mW/mA)
		0x01	// battery present voltage (mV)
	})

	/* Status */
	Method(_STA, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			If(\_SB.PCI0.LPCB.EC0.B2PR) {
				If(\_SB.PCI0.LPCB.EC0.B2CH) {
					Return(0x1f)
				}
			}
		}
		Return(0x0f)
	}

	/* Battery Info */
	Method(_BIF, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			Store (\_SB.PCI0.LPCB.EC0.B2DW, Index(PBIF, 1))
			Store (\_SB.PCI0.LPCB.EC0.B2FW, Index(PBIF, 2))
			Store (\_SB.PCI0.LPCB.EC0.B2DV, Index(PBIF, 4))
		}

		Return(PBIF)
	}

	/* Battery Status */
	Method(_BST, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			Store (\_SB.PCI0.LPCB.EC0.B2PW, Local0)
			If (LGreaterEqual (Local0, 0x8000)) {
				Subtract (0x10000, Local0, Local0)
			}
			Store (Local0, Index(PBST, 1))
			Store (\_SB.PCI0.LPCB.EC0.B2PV, Index(PBST, 3))

			Store (\_SB.PCI0.LPCB.EC0.B2RW, Index(PBST, 2))

			If (\_SB.PCI0.LPCB.EC0.ACCH) {
				If (\_SB.PCI0.LPCB.EC0.B2CH) {
					If (\_SB.PCI0.LPCB.EC0.B2CG) {
						Store (2, Index(PBST, 0))
					}
				}
			} Else {
				If (\_SB.PCI0.LPCB.EC0.B2LO) {
					Store (5, Index(PBST, 0))
				} Else {
					Store (1, Index(PBST, 0))
				}
			}
		}

		Return(PBST)
	}

}

Device (ADP1)
{
	Name (_HID, "ACPI0003")
	Method (_PSR, 0)
	{
		Store (\_SB.PCI0.LPCB.EC0.ACCH, PWRS)
		Stall (0x02)
		Return (PWRS)
	}

	Method (_PCL, 0)
	{
		Return (_SB)
	}
}

