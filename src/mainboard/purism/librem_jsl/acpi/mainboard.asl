/* SPDX-License-Identifier: GPL-2.0-only */

#define EC_GPE_SWI	0x6E	/* eSPI SCI */

Scope (\_SB.PCI0.LPCB)
{
	#include "ec.asl"
}
