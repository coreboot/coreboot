/* SPDX-License-Identifier: GPL-2.0-only */

Device (TCHG)
{
	Name (_HID, "INT3403")
	Name (_UID, 0)
	Name (PTYP, 0x0B)
	Name (_STR, Unicode("Battery Charger"))

	Method (_STA)
	{
		If (\DPTE == 1) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	/* Return charger performance states defined by mainboard */
	Method (PPSS)
	{
		Return (\_SB.CHPS)
	}

	/* Return maximum charger current limit */
	Method (PPPC)
	{
		/* Convert size of PPSS table to index */
		Local0 = SizeOf (\_SB.CHPS)
		Local0--

		/* Check if charging is disabled (AC removed) */
		If (\PWRS == 0) {
			/* Return last power state */
			Return (Local0)
		} Else {
			/* Return highest power state */
			Return (0)
		}

		Return (0)
	}

	/* Set charger current limit */
	Method (SPPC, 1)
	{
		/* Retrieve Control (index 4) for specified PPSS level */
		Local0 = DerefOf (DerefOf (\_SB.CHPS[ToInteger (Arg0)])[4])

		/* Pass Control value to EC to limit charging */
		\_SB.PCI0.LPCB.EC0.CHGS (Local0)
	}

	/* Initialize charger participant */
	Method (INIT)
	{
		/* Disable charge limit */
		\_SB.PCI0.LPCB.EC0.CHGD ()
	}
}
