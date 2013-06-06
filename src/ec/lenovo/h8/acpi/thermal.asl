Scope(\_TZ)
{
#if defined (CONFIG_BOARD_LENOVO_X201) && CONFIG_BOARD_LENOVO_X201
	Name (MEBT, 0)
#endif

	Method(C2K, 1, NotSerialized)
	{
		Multiply(Arg0, 10, Local0)
		Add (Local0, 2732, Local0)
		if (LLessEqual(Local0, 2732)) {
		        Return (3000)
		}

		if (LGreater(Local0, 4012)) {
		        Return (3000)
		}
		Return (Local0)
	}

	ThermalZone(THM0)
	{
		Method(_CRT, 0, NotSerialized) {
			Return (C2K(127))
		}
		Method(_TMP) {
#if defined (CONFIG_BOARD_LENOVO_X201) && CONFIG_BOARD_LENOVO_X201
		        /* Avoid tripping alarm if ME isn't booted at all yet */
		        If (LAnd (LNot (MEBT), LEqual (\_SB.PCI0.LPCB.EC.TMP0, 128))) {
                            Return (C2K(40))
                        }
			Store (1, MEBT)
#endif
			Return (C2K(\_SB.PCI0.LPCB.EC.TMP0))
		}
	}

	ThermalZone(THM1)
	{
		Method(_CRT, 0, NotSerialized) {
			Return (C2K(99))
		}

		Method(_PSV, 0, NotSerialized) {
			Return (C2K(94))
		}

		Method(_TMP) {
			Return (C2K(\_SB.PCI0.LPCB.EC.TMP1))
		}
	}
}
