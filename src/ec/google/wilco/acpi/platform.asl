/* SPDX-License-Identifier: GPL-2.0-only */

/* Call from \_SB._PTS() */
Method (PTS, 1, Serialized)
{
	Printf ("EC _PTS")
	If (EREG){
		W (FPTS, Arg0)
	}
}

/* Call from \_SB._WAK() */
Method (WAK, 1, Serialized)
{
	Printf ("EC _WAK")
	If (EREG){
		W (FWAK, Arg0)

		/* Indicate to EC that OS is ready for queries */
		W (ERDY, 1)

		/* Indicate that the OS supports S0ix */
		W (CSOS, 1)

		/* Tell EC to stop emulating PS/2 mouse */
		W (PS2M, 0)

		/* Enable DPTF support if enabled in devicetree */
		If (\DPTE == 1) {
			W (DWST, 1)
		}
	}
}
