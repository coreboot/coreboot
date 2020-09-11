/* SPDX-License-Identifier: GPL-2.0-only */

Device(EC)
{
	Name(_HID, EISAID("PNP0C09"))
	Name(_UID, 0)

	Name(_GPE, 0x17)
	Mutex(ECLK, 0)

	OperationRegion(ERAM, EmbeddedControl, 0x00, 0x100)
	Field(ERAM, ByteAcc, NoLock, Preserve)
	{
		Offset(0x01),
				LIDS, 1,        /* Lid status */
				HPAC, 1,
		Offset(0x02),
				WKLD, 1,        /* Lid wake */

		Offset(0x20),
				SPTR,   8,
				SSTS,   8,
				SADR,   8,
				SCMD,   8,
				SBFR,   256,
	}
        Field(ERAM, ByteAcc, Lock, Preserve)
        {
		Offset(0x24),
				SBDW,   16,
	}

	Method(SBPC, 0, NotSerialized)
	{
		Local0 = 1000
		While(Local0)
		{
			If(SPTR == 0)
			{
				Return()
			}

			Sleep(1)
			Local0--
		}
	}

	Method(SBRW, 2, NotSerialized)
	{
		Acquire(ECLK, 0xFFFF)
		SADR = (Arg0 << 1)
		SCMD = Arg1
		SPTR = 0x09
		SBPC()
		Local0 = SBDW
		Release(ECLK)
		Return(Local0)
	}

	Method(SBRB, 2, NotSerialized)
	{
		Acquire(ECLK, 0xFFFF)
		SADR = (Arg0 << 1)
		SCMD = Arg1
		SPTR = 0x0B
		SBPC()
		Local0 = SBFR
		Release(ECLK)
		Return(Local0)
	}

	/* LID status change. */
	Method(_Q20, 0, NotSerialized)
	{
		Notify(LID, 0x80)
	}

	/* AC status change. */
	Method(_Q21, 0, NotSerialized)
	{
		Notify(AC, 0x80)
	}

	Method(_CRS, 0)
	{
		Name(ECMD, ResourceTemplate()
		{
			IO(Decode16, 0x62, 0x62, 1, 1)
			IO(Decode16, 0x66, 0x66, 1, 1)
		})
		Return(ECMD)
	}

	Method(_INI, 0, NotSerialized)
	{
	}

	Device(LID)
	{
		Name(_HID, "PNP0C0D")

		Method(_LID, 0, NotSerialized)
		{
			return(LIDS)
		}

		Method(_PRW, 0, NotSerialized)
		{
			Return (Package() { 0x1d, 0x03 })
		}

		Method(_PSW, 1, NotSerialized)
		{
			if (Arg0) {
				WKLD = 1
			} else {
				WKLD = 0
			}
		}
	}

	Device(AC)
	{
		Name(_HID, "ACPI0003")
		Name(_UID, 0x00)
		Name(_PCL, Package() { \_SB } )

		Method(_PSR, 0, NotSerialized)
		{
			return(HPAC)
		}

		Method(_STA, 0, NotSerialized)
		{
			Return(0x0f)
		}
	}

	Device(BAT0)
	{
		Name(_HID, EisaId("PNP0C0A"))
		Name(_UID, 0x00)
		Name(_PCL, Package() { \_SB })

		Name(BATS, Package()
		{
			0x00,			// 0: PowerUnit: Report in mWh
			0xFFFFFFFF,		// 1: Design cap
			0xFFFFFFFF,		// 2: Last full charge cap
			0x01,			// 3: Battery Technology
			10800,			// 4: Design Voltage(mV)
			0x00,			// 5: Warning design capacity
			200,			// 6: Low design capacity
			10,			// 7: granularity1
			10,			// 8: granularity2
			"",			// 9: Model number
			"",			// A: Serial number
			"",			// B: Battery Type
			""			// C: OEM information
		})

		Name(BATI, Package()
		{
			0,			// Battery State
						// Bit 0 - discharge
						// Bit 1 - charge
						// Bit 2 - critical state
			0,			// Battery present Rate
			0,			// Battery remaining capacity
			0			// Battery present voltage
		})

		Method(_BIF, 0, NotSerialized)
		{
			BATS [0x01] = (SBRW (0x0B, 0x18) * 10)
			BATS [0x02] = (SBRW (0x0B, 0x10) * 10)
			BATS [0x04] = SBRW (0x0B, 0x19)
			BATS [0x09] = SBRB (0x0B, 0x21)
			BATS [0x0B] = SBRB (0x0B, 0x22)
			BATS [0x0C] = SBRB (0x0B, 0x20)

			Return(BATS)
		}

		Method(_STA, 0, NotSerialized)
		{
			If (SBRW(0x0A, 1) & 1) {
				Return(0x1f)
			} else {
				Return(0x0f)
			}
		}

		Method(_BST, 0, NotSerialized)
		{
			/* Check for battery presence.  */
			If (!(SBRW (0x0A, 1) & 1)) {
				Return(Package(4) {
				       0,
				       0xFFFFFFFF,
				       0xFFFFFFFF,
				       0xFFFFFFFF
				})
			}
			Local1 = SBRW(0x0B, 0x09)
			BATI [3] = Local1
			Local0 = SBRW (0x0B, 0x0A)
			/* Sign-extend Local0.  */
			If(Local0 & 0x8000)
			{
				Local0 = ~Local0
				Local0 = (Local0++ & 0xFFFF)
			}

			Local0 *= Local1
			BATI [1] = (Local0 / 1000)
			BATI [2] = (SBRW (0x0B, 0x0F) * 10)
			If(HPAC)
			{
				If (!(SBRW (0x0B, 0x16) & 0x40)) {
					BATI [0] = 2
				} Else {
					BATI [0] = 0
				}
			}
			Else
			{
				BATI [0] = 1
			}

			Return(BATI)
		}
	}
}
