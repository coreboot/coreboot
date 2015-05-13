/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <soc/iomap.h>

Scope (\)
{
	/* IO-Trap at 0x800.
	 * This is the ACPI->SMI communication interface.
	 */
	OperationRegion (IO_T, SystemIO, 0x800, 0x10)
	Field (IO_T, ByteAcc, NoLock, Preserve)
	{
		Offset (0x8),
		TRP0, 8		/* IO-Trap at 0x808 */
	}
}

/* PCI Express Ports 0:1c.x */
#include "pcie.asl"

/* USB XHCI 0:14.0 */
#include "xhci.asl"

/* LPC Bridge 0:1f.0 */
#include "lpc.asl"

/* SMBus 0:1f.3 */
#include "smbus.asl"

/* Serial IO */
#include "serialio.asl"

/* Interrupt Routing */
#include "itss.asl"
#include "irqlinks.asl"

Method (_OSC, 4)
{
	/* Check for proper GUID */
	If (LEqual (Arg0, ToUUID ("33DB4D5B-1FF7-401C-9657-7441C03DD766")))
	{
		/* Let OS control everything */
		Return (Arg3)
	}
	Else
	{
		/* Unrecognized UUID */
		CreateDWordField (Arg3, 0, CDW1)
		Or (CDW1, 4, CDW1)
		Return (Arg3)
	}
}
