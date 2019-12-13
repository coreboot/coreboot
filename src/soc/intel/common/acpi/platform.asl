/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2016 Intel Corp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <include/console/post_codes.h>

External(\_SB.MPTS, MethodObj)
External(\_SB.MWAK, MethodObj)
External(\_SB.PCI0.EGPM, MethodObj)
External(\_SB.PCI0.RGPM, MethodObj)

/* Port 80 POST */

OperationRegion (POST, SystemIO, CONFIG_POST_IO_PORT, 1)
Field (POST, ByteAcc, Lock, Preserve)
{
	DBG0, 8
}

/*
 * The _PTS method (Prepare To Sleep) is called before the OS is
 * entering a sleep state. The sleep state number is passed in Arg0
 */

Method (_PTS, 1)
{
	Store (POST_OS_ENTER_PTS, DBG0)

	If (CondRefOf (\_SB.MPTS))
	{
		\_SB.MPTS (Arg0)
	}
	/*
	 * Save the current PM bits then
	 * enable GPIO PM with MISCCFG_ENABLE_GPIO_PM_CONFIG
 	 */
	If (CondRefOf (\_SB.PCI0.EGPM))
	{
		\_SB.PCI0.EGPM ()
	}
}

/* The _WAK method is called on system wakeup */

Method (_WAK, 1)
{
	Store (POST_OS_ENTER_WAKE, DBG0)

	If (CondRefOf (\_SB.MWAK))
	{
		\_SB.MWAK (Arg0)
	}
	/* Restore GPIO all Community PM */
	If (CondRefOf (\_SB.PCI0.RGPM))
	{
		\_SB.PCI0.RGPM ()
	}

	Return (Package(){0,0})
}
