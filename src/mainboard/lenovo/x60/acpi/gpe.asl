Scope (\_GPE)
{
	Method(_L18, 0, NotSerialized)
	{
		/* Read EC register to clear wake status */
		Store(\_SB.PCI0.LPCB.EC.WAKE, Local0)
	}
}
