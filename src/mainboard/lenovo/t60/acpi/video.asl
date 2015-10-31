/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2011 Sven Schnelle <svens@stackframe.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
