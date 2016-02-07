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
Scope (\_GPE)
{
	Method(_L18, 0, NotSerialized)
	{
		/* Read EC register to clear wake status */
		Store(\_SB.PCI0.LPCB.EC.WAKE, Local0)
		/* So that we don't get a warning that Local0 is unused.  */
		Increment (Local0)
	}
}
