/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2015 Google Inc.
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
#include <soc/irq.h>
#include <soc/gpio_defs.h>
#include <soc/gpe.h>
#include <soc/pcr.h>

/* GPIO Controller */
#include "gpio.asl"

/* Interrupt Routing */
#include "irqlinks.asl"

/* LPC 0:1f.0 */
#include "lpc.asl"

/* PCIE Ports */
#include "pcie.asl"

/* PCR Access */
#include "pcr.asl"

/* PMC 0:1f.2 */
#include "pmc.asl"

/* Serial IO */
#include "serialio.asl"

/* SMBus 0:1f.3 */
#include "smbus.asl"

/* Storage Controllers */
#include "scs.asl"

/* USB XHCI 0:14.0 */
#include "xhci.asl"

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
