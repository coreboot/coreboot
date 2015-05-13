/*
 * This file is part of the coreboot project.
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
 */

/* ITSS */
/* Define the needed ITSS registers used by ASL on Interrupt */

Scope (\_SB)
{
	OperationRegion (ITSS, SystemMemory, 0xfdc43100, 0x8)
	Field (ITSS, ByteAcc, NoLock, Preserve)
	{
		PARC, 8,
		PBRC, 8,
		PCRC, 8,
		PDRC, 8,
		PERC, 8,
		PFRC, 8,
		PGRC, 8,
		PHRC, 8,
	}

	/*
	 * Pin# = group_pad# + group# * 24.
	 * For instance, GPP_A_6 would be pin#6,
	 * 	GPP_D_23 would be 23+(3*24), pin#95.
	 */
	Name (GPPG, Package (0x02)
	{
		Package (0x08)
		{
			0x18,
			0x18,
			0x18,
			0x18,
			0x18,
			0x18,
			0x08,
			0x0C
		},

		Package (0x0A)
		{
			0x18,
			0x18,
			0x18,
			0x18,
			0x0D,
			0x18,
			0x18,
			0x18,
			0x0B,
			0x0C
		}
	})

	Method (GNMB, 1, Serialized)
	{
		Return (And (Arg0, 0xFFFF))
	}

	Method (GGRP, 1, Serialized)
	{
		ShiftRight (And (Arg0, 0x00FF0000), 0x10, Local0)
		Return (Local0)
	}

	/* Convert GPIO PAD name to GPIO number */
	Method (INUM, 1, NotSerialized)
	{
		Store (One, Local0)
		Store (GNMB (Arg0), Local1)
		Store (GGRP (Arg0), Local2)
		Store (Zero, Local3)

		While (LLess (Local3, Local2))
		{
			Add (DerefOf (Index (DerefOf (Index
					(GPPG, Local0)), Local3)),
					Local1, Local1)
			Increment (Local3)
		}

		Return (Add (0x18, Mod (Local1, 0x60)))
	}
}
