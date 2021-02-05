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

	/* Return charger performance states defined by Terra2 or Terra3 mainboard */
	Method (PPSS)
	{
		If (\_SB.GPID == TERRA2_PROJECT_ID)
		{
			Return (\_SB.CPT2)
		} Else {
			Return (\_SB.CPT3)
		}
	}

	/* Return maximum charger current limit */
	Method (PPPC)
	{
		/* Convert size of PPSS table to index */
		If (\_SB.GPID == TERRA2_PROJECT_ID)
		{
			Local0 = SizeOf (\_SB.CPT2)
		} Else {
			Local0 = SizeOf (\_SB.CPT3)
		}

		Local0--

		/* Check if charging is disabled (AC removed) */
		If (\_SB.PCI0.LPCB.EC0.ACEX == 0) {
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
		/* Convert size of PPSS table to index */
		If (\_SB.GPID == TERRA2_PROJECT_ID)
		{
			Local0 = DeRefOf (DeRefOf (\_SB.CPT2 [ToInteger (Arg0)])[4])
		} Else {
			Local0 = DeRefOf (DeRefOf (\_SB.CPT3 [ToInteger (Arg0)])[4])
		}

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
