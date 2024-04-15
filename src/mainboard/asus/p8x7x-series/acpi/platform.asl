/* SPDX-License-Identifier: GPL-2.0-only */

Method(_PTS, 1)
{
#if (CONFIG(BOARD_ASUS_P8Z77_M) || CONFIG(BOARD_ASUS_P8Z77_M_PRO))
	/* blink power LED if not turning off */
	If (Arg0 != 0x05)
	{
	    GB27 = 1
	}
#endif
}

Method(_WAK, 1)
{
#if (CONFIG(BOARD_ASUS_P8Z77_M) || CONFIG(BOARD_ASUS_P8Z77_M_PRO))
	GB27 = 0
#endif
	Return(Package(){0, 0})
}
