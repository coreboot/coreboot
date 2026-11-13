/* SPDX-License-Identifier: GPL-2.0-only */

/* Display outputs of the discrete GPU, like the T61 vendor BIOS */
Scope (\_SB.PCI0.PEGP.DGPU)
{
	Method (_DOS, 1, NotSerialized)
	{
	}

	Method (_DOD, 0, NotSerialized)
	{
		Return (Package () { 0x0100, 0x0110, 0x0120 })
	}

	Device (CRT0)
	{
		Name (_ADR, 0x0100)
	}

	OperationRegion (DGPC, PCI_Config, 0x10, 4)
	Field (DGPC, DWordAcc, NoLock, Preserve)
	{
		BAR0, 32,
	}

	Device (LCD0)
	{
		Name (_ADR, 0x0110)

		/* Brightness levels in percent, from the vendor BIOS _BCL */
		Name (BCLP, Package () {
			20, 25, 30, 35, 40, 45, 50, 55,
			60, 65, 70, 75, 80, 85, 90, 100
		})

		/*
		 * Backlight PWM duty cycle out of 255 for each level, from the BRTB
		 * table of the vendor BIOS. At a lower duty cycle, the CCFL backlight
		 * only lights up part of the panel.
		 */
		Name (BCLD, Package () {
			0x25, 0x4e, 0x5c, 0x69, 0x75, 0x81, 0x8c, 0x97,
			0xa2, 0xad, 0xb8, 0xc6, 0xd3, 0xe1, 0xf0, 0xff
		})

		/* LVDS on SOR0 of the G8x GPU, the duty cycle is out of 1025 */
		Method (PWMR, 0, Serialized)
		{
			OperationRegion (SPWM, SystemMemory, (^^BAR0 & 0xfffffff0) + 0x61c084, 4)
			Field (SPWM, DWordAcc, NoLock, Preserve)
			{
				PWMC, 32,
			}
			Return (PWMC & 0x7ff)
		}

		Method (PWMW, 1, Serialized)
		{
			OperationRegion (SPWM, SystemMemory, (^^BAR0 & 0xfffffff0) + 0x61c084, 4)
			Field (SPWM, DWordAcc, NoLock, Preserve)
			{
				PWMC, 32,
			}
			/* Bit 31 makes the GPU latch the new value */
			PWMC = (1 << 31) | Arg0
		}

		Method (_BCL, 0, NotSerialized)
		{
			Return (Package () {
				100, 100,
				20, 25, 30, 35, 40, 45, 50, 55,
				60, 65, 70, 75, 80, 85, 90, 100
			})
		}

		Method (_BCM, 1, NotSerialized)
		{
			Local0 = Match (BCLP, MEQ, Arg0, MTR, 0, 0)
			If (Local0 != Ones) {
				PWMW (DerefOf (BCLD[Local0]) * 1025 / 255)
			}
		}

		Method (_BQC, 0, NotSerialized)
		{
			Local0 = PWMR ()
			Local1 = SizeOf (BCLP)
			While (Local1 > 0) {
				Local1--
				If (DerefOf (BCLD[Local1]) * 1025 / 255 <= Local0) {
					Break
				}
			}
			Return (DerefOf (BCLP[Local1]))
		}
	}

	Device (DVI0)
	{
		Name (_ADR, 0x0120)
	}
}

/*
 * Brightness keys: the IGD changes the backlight level itself, with a
 * discrete GPU the OS does it.
 */
Scope (\)
{
	Method (BRTU, 0, NotSerialized)
	{
		If (\_SB.PCI0.MCHC.IGDE) {
			\_SB.PCI0.GFX0.INCB ()
		} Else {
			Notify (\_SB.PCI0.PEGP.DGPU.LCD0, 0x86)
		}
	}

	Method (BRTD, 0, NotSerialized)
	{
		If (\_SB.PCI0.MCHC.IGDE) {
			\_SB.PCI0.GFX0.DECB ()
		} Else {
			Notify (\_SB.PCI0.PEGP.DGPU.LCD0, 0x87)
		}
	}
}
