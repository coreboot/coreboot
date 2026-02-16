/* SPDX-License-Identifier: GPL-2.0-only */

#if CONFIG(STARLABS_ACPI_EFI_OPTION_SMI)
Field (\DNVS, ByteAcc, NoLock, Preserve)
{
	EOCM, 32,	// EFI option command
	EOID, 32,	// EFI option ID
	EOVL, 32,	// EFI option value
	EORS, 32,	// EFI option status
}

Name (EOAP, 0xE2)	// STARLABS_APMC_CMD_EFI_OPTION

Name (EOFL, 0x1)	// STARLABS_EFIOPT_ID_FN_LOCK_STATE
Name (EOTP, 0x2)	// STARLABS_EFIOPT_ID_TRACKPAD_STATE
Name (EOKB, 0x3)	// STARLABS_EFIOPT_ID_KBL_BRIGHTNESS
Name (EOKS, 0x4)	// STARLABS_EFIOPT_ID_KBL_STATE

Mutex (EOMX, 0x00)

Method (EOGT, 1, Serialized)
{
	If (Acquire (EOMX, 1000))
	{
		Return (0xFFFFFFFF)
	}

	Store (0x01, EOCM)
	Store (Arg0, EOID)
	Store (0x00, EORS)
	Store (EOAP, \_SB.PCI0.LPCB.EC.SMB2)

	Store (EOVL, Local0)
	Release (EOMX)
	Return (Local0)
}

Method (EOSV, 2, Serialized)
{
	If (Acquire (EOMX, 1000))
	{
		Return (0x01)
	}

	Store (0x02, EOCM)
	Store (Arg0, EOID)
	Store (Arg1, EOVL)
	Store (0x00, EORS)
	Store (EOAP, \_SB.PCI0.LPCB.EC.SMB2)

	Store (EORS, Local0)
	Release (EOMX)
	Return (Local0)
}
#endif

Method (RPTS, 1, Serialized)
{

#if CONFIG(STARLABS_ACPI_EFI_OPTION_SMI)
	/* Store current EC settings in UEFI variable store */
	Store (\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.TPLE)), Local0)
	If (Local0 == 0x11)
	{
		Store (0x00, Local0)
	}
	EOSV (EOTP, Local0)

	EOSV (EOFL, \_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.FLKE)))
	EOSV (EOKS, \_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.KLSE)))
	EOSV (EOKB, \_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.KLBE)))
#else
	/* Store current EC settings in CMOS */
	Switch (ToInteger (\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.TPLE))))
	{
		// 0x00 == Enabled    == 0x00
		// 0x11 == Re-enabled == 0x00
		// 0x22 == Disabled   == 0x01
		Case (0x00)
		{
			Store (0x00, \_SB.PCI0.LPCB.TPLC)
		}
		Case (0x11)
		{
			Store (0x00, \_SB.PCI0.LPCB.TPLC)
		}
		Case (0x22)
		{
			Store (0x01, \_SB.PCI0.LPCB.TPLC)
		}
	}

	Store (\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.FLKE)), \_SB.PCI0.LPCB.FLKC)

	Switch (ToInteger (\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.KLSE))))
	{
		// 0x00 == Disabled == 0x00
		// 0xdd == Enabled  == 0x01
		Case (0x00)
		{
			Store (0x00, \_SB.PCI0.LPCB.KLSC)
		}
		Case (0xdd)
		{
			Store (0x01, \_SB.PCI0.LPCB.KLSC)
		}
	}

	Switch (ToInteger (\_SB.PCI0.LPCB.EC.ECRD (RefOf (\_SB.PCI0.LPCB.EC.KLBE))))
	{
		// 0xdd == On   == 0x00
		// 0xcc == Off  == 0x01
		// 0xbb == Low  == 0x02
		// 0xaa == High == 0x03
		Case (0xdd)
		{
			Store (0x00, \_SB.PCI0.LPCB.KLBC)
		}
		Case (0xcc)
		{
			Store (0x01, \_SB.PCI0.LPCB.KLBC)
		}
		Case (0xbb)
		{
			Store (0x02, \_SB.PCI0.LPCB.KLBC)
		}
		Case (0xaa)
		{
			Store (0x03, \_SB.PCI0.LPCB.KLBC)
		}
	}
#endif

	/*
	 * Disable ACPI support.
	 * This should always be the last action before entering a sleep state.
	 */
	\_SB.PCI0.LPCB.EC.ECWR(0x00, RefOf(\_SB.PCI0.LPCB.EC.OSFG))

	Return (Arg0)
}

Method (RWAK, 1, Serialized)
{
	/*
	 * Enable ACPI support.
	 * This should always be the first action when exiting a sleep state.
	 */
	\_SB.PCI0.LPCB.EC.ECWR(0x01, RefOf(\_SB.PCI0.LPCB.EC.OSFG))

#if CONFIG(STARLABS_ACPI_EFI_OPTION_SMI)
	/* Restore EC settings from UEFI variable store */
	Store (EOGT (EOTP), Local0)
	If (Local0 == 0x22)
	{
		\_SB.PCI0.LPCB.EC.ECWR (0x22, RefOf(\_SB.PCI0.LPCB.EC.TPLE))
	}
	Else
	{
		\_SB.PCI0.LPCB.EC.ECWR (0x00, RefOf(\_SB.PCI0.LPCB.EC.TPLE))
	}

	\_SB.PCI0.LPCB.EC.ECWR (EOGT (EOFL), RefOf(\_SB.PCI0.LPCB.EC.FLKE))

	Store (EOGT (EOKS), Local0)
	If (Local0 == 0xdd)
	{
		\_SB.PCI0.LPCB.EC.ECWR (0xdd, RefOf(\_SB.PCI0.LPCB.EC.KLSE))
	}
	Else
	{
		\_SB.PCI0.LPCB.EC.ECWR (0x00, RefOf(\_SB.PCI0.LPCB.EC.KLSE))
	}

	Store (EOGT (EOKB), Local0)
	Switch (ToInteger (Local0))
	{
		Case (0xdd)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xdd, RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
		Case (0xcc)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xcc, RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
		Case (0xbb)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xbb, RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
		Case (0xaa)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xaa, RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
	}
#else
	/* Restore EC settings from CMOS */
	Switch (ToInteger (\_SB.PCI0.LPCB.TPLC))
	{
		// 0x00 == Enabled    == 0x00
		// 0x00 == Re-enabled == 0x11
		// 0x01 == Disabled   == 0x22
		Case (0x00)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0x00,  RefOf(\_SB.PCI0.LPCB.EC.TPLE))
		}
		Case (0x01)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0x22,  RefOf(\_SB.PCI0.LPCB.EC.TPLE))
		}
	}

	\_SB.PCI0.LPCB.EC.ECWR (\_SB.PCI0.LPCB.FLKC,  RefOf(\_SB.PCI0.LPCB.EC.FLKE))

	Switch (ToInteger (\_SB.PCI0.LPCB.KLSC))
	{
		// 0x00 == Disabled == 0x00
		// 0x01 == Enabled  == 0xdd
		Case (0x00)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0x00,  RefOf(\_SB.PCI0.LPCB.EC.KLSE))
		}
		Case (0x01)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xdd,  RefOf(\_SB.PCI0.LPCB.EC.KLSE))
		}
	}

	Switch (ToInteger (\_SB.PCI0.LPCB.KLBC))
	{
		// 0x00 == On   == 0xdd
		// 0x01 == Off  == 0xcc
		// 0x02 == Low  == 0xbb
		// 0x03 == High == 0xaa
		Case (0x00)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xdd,  RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
		Case (0x01)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xcc,  RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
		Case (0x02)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xbb,  RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
		Case (0x03)
		{
			\_SB.PCI0.LPCB.EC.ECWR (0xaa,  RefOf(\_SB.PCI0.LPCB.EC.KLBE))
		}
	}
#endif

	Return (Arg0)
}
