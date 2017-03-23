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

	/* Get DW0 address of a given pad */
	Method (GDW0, 0x2, Serialized)
	{
		/* Arg0 - GPIO portid */
		/* Arg1 - GPIO pad offset relative to the community */
		Store (0, Local1)
		Or( Or (ShiftLeft (Arg0, 16), CONFIG_IOSF_BASE_ADDRESS),
					Local1, Local1)
		Or( Add (PAD_CFG_BASE, Multiply (Arg1, 8)), Local1, Local1)
		Return (Local1)
	}

	/* Calculate HOSTSW_REG address */
	Method (CHSA, 0x1, Serialized)
	{
		/* Arg0 - GPIO pad offset relative to the community */
		Add (HOSTSW_OWN_REG_BASE, Multiply (Divide (Arg0, 32), 4), Local1)
		Return (Local1)
	}

	/* Get Host ownership register of GPIO Community */
	Method (GHO, 0x2, Serialized)
	{
		/* Arg0 - GPIO portid */
		/* Arg1 - GPIO pad offset relative to the community */
		Store (CHSA (Arg1), Local1)

		OperationRegion (SHO0, SystemMemory, Or ( Or
			(CONFIG_IOSF_BASE_ADDRESS, ShiftLeft (Arg0, 16)), Local1), 4)
		Field (SHO0, AnyAcc, NoLock, Preserve) {
			TEMP, 32
		}
		Return (TEMP)
	}

	/* Set Host ownership register of GPIO Community */
	Method (SHO, 0x3, Serialized)
	{
		/* Arg0 - GPIO portid */
		/* Arg1 - GPIO pad offset relative to the community */
		/* Arg2 - Value for Host own register */
		Store (CHSA (Arg1), Local1)

		OperationRegion (SHO0, SystemMemory, Or ( Or
			(CONFIG_IOSF_BASE_ADDRESS, ShiftLeft (Arg0, 16)), Local1), 4)
		Field (SHO0, AnyAcc, NoLock, Preserve) {
			TEMP, 32
		}
		Store (Arg2, TEMP)
	}
}
