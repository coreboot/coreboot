/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB) {
	#include "sleep.asl"
}

Scope (_GPE)
{
	Method (_L0F, 0, NotSerialized)
	{
		\_SB.PCI0.LPCB.EC.VBTN.UPDK()
	}
}
