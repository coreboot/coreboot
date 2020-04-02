/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* simple name description */
/*
#include <arch/acpi.h>
DefinitionBlock ("DSDT.AML", "DSDT", 0x01, OEM_ID, ACPI_TABLE_CREATOR, 0x00010001
		)
	{
		#include "usb.asl"
	}
*/

/* USB overcurrent mapping pins.   */
Name(UOM0, 0)
Name(UOM1, 2)
Name(UOM2, 0)
Name(UOM3, 7)
Name(UOM4, 2)
Name(UOM5, 2)
Name(UOM6, 6)
Name(UOM7, 2)
Name(UOM8, 6)
Name(UOM9, 6)

/* USB Overcurrent GPEs */
/* TODO: Update for Gardenia */
