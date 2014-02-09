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

#if IS_ENABLED(CONFIG_BOARD_LENOVO_X60T)
	Device (DTR)		// Digitizer
	{
		Name(_HID, EISAID("WACF004"))

		Name(_CRS, ResourceTemplate()
		{
			IO (Decode16, 0x200, 0x200, 0x01, 0x08)
                        IRQNoFlags () {5}
		})

		Method (_STA, 0)
		{
			Return (0xf)
		}
	}
#endif
