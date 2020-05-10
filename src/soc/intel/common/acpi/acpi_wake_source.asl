/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB)
{
	Method (_SWS)
	{
		/* Index into PM1 for device that caused wake */
		Return (\PM1I)
	}
}

Scope (\_GPE)
{
	Method (_SWS)
	{
		/* Index into GPE for device that caused wake */
		Return (\GPEI)
	}
}
