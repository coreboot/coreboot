/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include "smi.h"

Scope (\)
{
	Method(BRTD, 0, NotSerialized)
	{
		Trap(SMI_BRIGHTNESS_DOWN)
		\_SB.PCI0.GFX0.DECB()
	}

	Method(BRTU, 0, NotSerialized)
	{
		Trap(SMI_BRIGHTNESS_UP)
		\_SB.PCI0.GFX0.INCB()
	}
}
