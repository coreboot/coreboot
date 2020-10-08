/* SPDX-License-Identifier: GPL-2.0-only */

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
			PBIF [1] = \_SB.PCI0.LPCB.EC0.B1DW
			PBIF [2] = \_SB.PCI0.LPCB.EC0.B1FW
			PBIF [4] = \_SB.PCI0.LPCB.EC0.B1DV
		}

		Return(PBIF)
	}

	/* Battery Status */
	Method(_BST, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			Local0 = \_SB.PCI0.LPCB.EC0.B1PW
			If (Local0 >= 0x8000) {
				Local0 = 0x10000 - Local0
			}
			PBST [1] = Local0
			PBST [3] = \_SB.PCI0.LPCB.EC0.B1PV

			PBST [2] = \_SB.PCI0.LPCB.EC0.B1RW

			If (\_SB.PCI0.LPCB.EC0.ACCH) {
				If (\_SB.PCI0.LPCB.EC0.B1CH) {
					If (\_SB.PCI0.LPCB.EC0.B1CG) {
						PBST [0] = 2
					}
				}
			} Else {
				If (\_SB.PCI0.LPCB.EC0.B1LO) {
					PBST [0] = 5
				} Else {
					PBST [0] = 1
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
			PBIF [1] = \_SB.PCI0.LPCB.EC0.B2DW
			PBIF [2] = \_SB.PCI0.LPCB.EC0.B2FW
			PBIF [4] = \_SB.PCI0.LPCB.EC0.B2DV
		}

		Return(PBIF)
	}

	/* Battery Status */
	Method(_BST, 0)
	{
		If(\_SB.PCI0.LPCB.EC0.ECON) {
			Local0 = \_SB.PCI0.LPCB.EC0.B2PW
			If (Local0 >= 0x8000) {
				Local0 = 0x10000 - Local0
			}
			PBST [1] = Local0
			PBST [3] = \_SB.PCI0.LPCB.EC0.B2PV

			PBST [2] = \_SB.PCI0.LPCB.EC0.B2RW

			If (\_SB.PCI0.LPCB.EC0.ACCH) {
				If (\_SB.PCI0.LPCB.EC0.B2CH) {
					If (\_SB.PCI0.LPCB.EC0.B2CG) {
						PBST [0] = 2
					}
				}
			} Else {
				If (\_SB.PCI0.LPCB.EC0.B2LO) {
					PBST [0] = 5
				} Else {
					PBST [0] = 1
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
		PWRS = \_SB.PCI0.LPCB.EC0.ACCH
		Stall (0x02)
		Return (PWRS)
	}

	Method (_PCL, 0)
	{
		Return (_SB)
	}
}
