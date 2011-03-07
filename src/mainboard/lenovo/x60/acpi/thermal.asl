Scope(\_TZ)
{
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
			Return (C2K(\_SB.PCI0.LPCB.EC.TMP0))
		}
	}
}
