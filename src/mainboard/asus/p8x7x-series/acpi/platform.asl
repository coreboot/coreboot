/* SPDX-License-Identifier: GPL-2.0-only */
#if CONFIG(POWER_LED_USES_GPIO8)
#define BLINK_POWER_LED GB08
#else
#define BLINK_POWER_LED GB27
#endif

Method(_PTS, 1)
{
	/* blink power LED if not turning off */
	If (Arg0 != 5)
	{
	    BLINK_POWER_LED = 1
	}
}

Method(_WAK, 1)
{
	BLINK_POWER_LED = 0
	Return(Package(){0, 0})
}
