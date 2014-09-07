	Method (_L01, 0, NotSerialized)
	{
		If (\_SB.PCI0.RP04.HPCS)
		{
			Sleep (100)
			Store (0x01, \_SB.PCI0.RP04.HPCS)
			If (\_SB.PCI0.RP04.PDC)
			{
				Store (0x01, \_SB.PCI0.RP04.PDC)
				Notify (\_SB.PCI0.RP04, 0x00)
			}
		}
	}
