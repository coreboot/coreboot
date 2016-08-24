/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

Scope (\_SB)
{
	/* Get Pad Configuration DW0 register value */
	Method (GPC0, 0x1, Serialized)
	{
		/* Arg0 - GPIO DW0 address */
		Store (Arg0, Local0)
		OperationRegion (PDW0, SystemMemory, Local0, 4)
		Field (PDW0, AnyAcc, NoLock, Preserve) {
			TEMP, 32
		}
		Return (TEMP)
	}

	/* Set Pad Configuration DW0 register value */
	Method (SPC0, 0x2, Serialized)
	{
		/* Arg0 - GPIO DW0 address */
		/* Arg1 - Value for DW0 register */
		Store (Arg0, Local0)
		OperationRegion (PDW0, SystemMemory, Local0, 4)
		Field (PDW0, AnyAcc, NoLock, Preserve) {
			TEMP,32
		}
		Store (Arg1, TEMP)
	}

	/* Get Pad Configuration DW1 register value */
	Method (GPC1, 0x1, Serialized)
	{
		/* Arg0 - GPIO DW0 address */
		Store (Add (Arg0, 0x4), Local0)
		OperationRegion (PDW1, SystemMemory, Local0, 4)
		Field (PDW1, AnyAcc, NoLock, Preserve) {
			TEMP, 32
		}
		Return (TEMP)
	}

	/* Set Pad Configuration DW1 register value */
	Method (SPC1, 0x2, Serialized)
	{
		/* Arg0 - GPIO DW0 address */
		/* Arg1 - Value for DW1 register */
		Store (Add (Arg0, 0x4), Local0)
		OperationRegion (PDW1, SystemMemory, Local0, 4)
		Field(PDW1, AnyAcc, NoLock, Preserve) {
			TEMP,32
		}
		Store (Arg1, TEMP)
	}
}
