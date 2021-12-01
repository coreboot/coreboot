/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef EC_GPE_SCI
#error EC_GPE_SCI must be defined by mainboard.
#endif
#ifndef EC_GPE_PWRB
#error EC_GPE_PWRB must be defined by mainboard.
#endif
#ifndef EC_GPE_SLPB
#error EC_GPE_PWRB must be defined by mainboard.
#endif
#ifndef EC_GPE_LID
#error EC_GPE_LID must be defined by mainboard.
#endif

Scope (\_SB)
{
	#include "ac.asl"
	#include "battery.asl"
	#include "buttons.asl"
	#include "lid.asl"
	#include "hid.asl"

	Scope (PCI0.LPCB)
	{
		#include "ec.asl"
	}
}
