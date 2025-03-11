/* SPDX-License-Identifier: GPL-2.0-or-later */

#define NBCI_FUNC_SUPPORT	0
#define NBCI_FUNC_GETOBJBYTYPE	16
#define NBCI_FUNC_GETCALLBACKS	19

#define GPS_FUNC_GETCALLBACKS	0x13

Method (NBCI, 2, Serialized)
{
	Switch (ToInteger (Arg0))
	{
		Case (NBCI_FUNC_SUPPORT)
		{
			Return (ITOB(
				(1 << NBCI_FUNC_SUPPORT) |
				(1 << NBCI_FUNC_GETCALLBACKS)))
		}
		Case (NBCI_FUNC_GETCALLBACKS)
		{
			/* Re-use the GPS subfunction's GETCALLBACKS Method */
			Return (GPS (GPS_FUNC_GETCALLBACKS, Arg1))
		}
	}

	Return (NV_ERROR_UNSUPPORTED)
}
