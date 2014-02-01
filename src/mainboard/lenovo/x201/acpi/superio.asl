#include "../../../../drivers/pc80/ps2_controller.asl"

#if IS_ENABLED(CONFIG_BOARD_LENOVO_X201T)
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
