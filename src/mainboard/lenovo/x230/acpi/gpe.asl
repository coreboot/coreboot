	Method (_L01, 0, NotSerialized)
	{
		If (\_SB.PCI0.RP03.HPCS)
		{
			Sleep (100)
			Store (0x01, \_SB.PCI0.RP03.HPCS)
			If (\_SB.PCI0.RP03.PDC)
			{
				Store (0x01, \_SB.PCI0.RP03.PDC)
				Notify (\_SB.PCI0.RP03, 0x00)
			}
		}
	}
