/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/clevo/it5570e/commands.h>

#define EC_DATA_IO	0x62
#define EC_SC_IO	0x66

Device (EC0)
{
	Name (_HID, "PNP0C09")
	Name (_GPE, EC_GPE_SCI)

	Name (_CRS, ResourceTemplate ()
	{
		IO (Decode16, 0x62, 0x62, 0, 1)
		IO (Decode16, 0x66, 0x66, 0, 1)
	})

	#include <ec/acpi/ec.asl>

	OperationRegion (PMS0, SystemMemory, 0xfe001b1c, 4)
	Field (PMS0, DWordAcc, NoLock, Preserve)
	{
		    , 15,
		GS0E,  1, /* Global SLP_S0# enable */
	}

	#include "ec_ram.asl"

	External (SFCV, MethodObj) /* Generated in SSDT */

	Method (INIT)
	{
		Printf ("EC: INIT")

		ECOS  = 2	/* ACPI with driver */
		BLCT  = 1	/* Enable ACPI brightness control */
		CAMK  = 1	/* Enable camera hotkey */

		SFCV ()		/* Apply custom fan curve */

		PNOT ()
	}

	Method (_INI)
	{
		Printf ("EC: _INI")

		INIT ()
	}

	/* Send FCMD */
	Method (SFCC, 1, Serialized)
	{
		FCMD = Arg0
		SEND_EC_COMMAND(0)

		/* EC sets FCMD = 0x00 on completion (FCMD = 0xfa on some commands) */
		Local0 = 50
		While (Local0--)
		{
			Stall (1)
			If (FCMD == 0x00 || FCMD == 0xfa)
			{
				Printf("EC: FCMD 0x%o completed after %o ms",
				       ToHexString(Arg0), ToDecimalString(50 - Local0))
				Return (1)
			}
		}
		Printf("EC: FCMD 0x%o timed out", ToHexString(Arg0))
		Return (0)
	}

	/*
	 * Method called from _PTS prior to system sleep state entry
	 */
	Method (PTS, 1, Serialized)
	{
		Printf ("EC: PTS: Arg0=%o", ToDecimalString(Arg0))

		WFNO = 0	/* Clear wake cause */
	}

	/*
	 * Method called from _WAK prior to system sleep state wakeup
	 */
	Method (WAK, 1, Serialized)
	{
		Printf ("EC: WAK: Arg0=%o, WFNO=%o", ToDecimalString(Arg0), ToHexString (WFNO))

		INIT ()

		/* update battery */
		Notify (\_SB.BAT0, 0x00) /* bus check */
		Notify (\_SB.BAT0, 0x80) /* state change */
		Notify (\_SB.BAT0, 0x81) /* information change */

		/* update AC */
		Notify (\_SB.AC, 0x00) /* bus check */
		Notify (\_SB.AC, 0x80) /* state change */

		If (Arg0 == 0x03 || Arg0 == 0x04) {
			Notify (\_SB.PWRB, 0x02) /* Wake */
		}
	}

	/*
	 * Display On/Off Notifications
	 * Called from \_SB.PEPD._DSM
	 */
	Name (KBLV, 0)
	Method (EDSX, 1, Serialized)
	{
		Printf ("EC: PEP display hook, state=%o", ToDecimalString (Arg0))

		If (S5FG)
		{
			Return ()
		}

		/* Display off */
		If (!Arg0)
		{
			/* Store current keyboard backlight level */
			FDAT = FDAT_KBLED_WHITE_GET_LEVEL
			SFCC (FCMD_KLED)
			KBLV = FBUF

			/* Turn off keyboard backlight */
			FDAT = FDAT_KBLED_WHITE_SET_LEVEL
			FBUF = 0x00
			SFCC (FCMD_KLED)
		}

		/* Display on */
		Else
		{
			/* Restore keyboard backlight level */
			FDAT = FDAT_KBLED_WHITE_SET_LEVEL
			FBUF = KBLV
			SFCC (FCMD_KLED)
		}
	}

	/*
	 * S0ix Entry/Exit Notifications
	 * Called from \_SB.PEPD._DSM
	 */
	Method (S0IX, 1, Serialized)
	{
		If (S5FG)
		{
			Return ()
		}

		Printf ("EC: S0ix change, state=%o", ToDecimalString (Arg0))

		/* S0ix entry */
		If (Arg0)
		{
			MSFG = 1	/* Notify EC */
		}

		/* S0ix exit */
		Else
		{
			GS0E = 0	/* Block SLP_S0# assertion during wakeup */
			MSFG = 0	/* Notfiy EC */
			Sleep (150)	/* wait for EC to become ready */
			SFCV ()		/* Apply custom fan curve */
			GS0E = 1	/* Unblock SLP_S0# */
		}
	}

	#include "ec_queries.asl"
}
