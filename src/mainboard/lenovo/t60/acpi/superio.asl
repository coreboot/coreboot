	Device (FIR)		// Infrared
	{
		Name(_HID, EISAID("IBM0071"))
		Name(_CID, EISAID("PNP0511"))

		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x2f8, 0x2f8, 0x01, 0x08)
                        IRQNoFlags () {3}
		})

		Method (_STA, 0)
		{
			Return (0xf)
		}
	}
