/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

/* This file defines the processor and performance state capability
 * for each core in the system.  It is included into the DSDT for each
 * core.  It assumes that each core of the system has the same performance
 * charateristics.
*/
/*
#include <arch/acpi.h>
DefinitionBlock ("DSDT.AML", "DSDT", 0x01, OEM_ID, ACPI_TABLE_CREATOR, 0x00010001)
	{
	Scope (\_PR) {
		Device (CPU0) {
		Name (_HID, "ACPI0007")
		Name (_UID, 0)
			#include "cpstate.asl"
		}
		Device (CPU1) {
		Name (_HID, "ACPI0007")
		Name (_UID, 1)
			#include "cpstate.asl"
		}
		Device (CPU2) {
		Name (_HID, "ACPI0007")
		Name (_UID, 2)
			#include "cpstate.asl"
		}
		Device (CPU3) {
		Name (_HID, "ACPI0007")
		Name (_UID, 3)
			#include "cpstate.asl"
		}
	}
*/
	/* P-state support: The maximum number of P-states supported by the */
	/* CPUs we'll use is 6. */
	/* Get from AMI BIOS. */
	Name(_PSS, Package(){
		Package ()
		{
		    0x00000AF0,
		    0x0000BF81,
		    0x00000002,
		    0x00000002,
		    0x00000000,
		    0x00000000
		},

		Package ()
		{
		    0x00000578,
		    0x000076F2,
		    0x00000002,
		    0x00000002,
		    0x00000001,
		    0x00000001
		}
	})

	Name(_PCT, Package(){
		ResourceTemplate(){Register(FFixedHW, 0, 0, 0)},
		ResourceTemplate(){Register(FFixedHW, 0, 0, 0)}
	})

	Method(_PPC, 0){
		Return(0)
	}
