/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Advanced Micro Devices, Inc.
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

#include <soc/iomap.h>

/* Get pin control MMIO address */
Method (GPAD, 0x1)
{
	/* Arg0 - GPIO pin number */
	Return (Add(Multiply(Arg0, 4), GPIO_CONTROL_BASE))
}

/* Read pin control dword */
Method (GPRD, 0x1, Serialized)
{
	/* Arg0 - GPIO pin control MMIO address */
	Store (Arg0, Local0)
	OperationRegion (GPDW, SystemMemory, Local0, 4)
	Field (GPDW, AnyAcc, NoLock, Preserve) {
		TEMP, 32
	}
	Return (TEMP)
}

/* Write pin control dword */
Method (GPWR, 0x2, Serialized)
{
	/* Arg0 - GPIO pin control MMIO address */
	/* Arg1 - Value for control register */
	Store (Arg0, Local0)
	OperationRegion (GPDW, SystemMemory, Local0, 4)
	Field (GPDW, AnyAcc, NoLock, Preserve) {
		TEMP,32
	}
	Store (Arg1, TEMP)
}

Method (GPGB, 0x2)
{
	/*
	 * Returns the desired byte
	 * Arg0 - GPIO pin control MMIO address
	 * Arg1 - Desired byte (0 through 3)
	 */
	Store (Multiply(Arg1, 8), Local2)
	Return (And(ShiftRight(GPRD(Arg0), Local2), 0x000000FF))
}

Method (GPSB, 0x3)
{
	/*
	 * Reads dword, replace byte, write back dword
	 * Arg0 - GPIO pin control MMIO address
	 * Arg1 - Desired byte (0 through 3)
	 * Arg2 - Value
	 */
	Store (Multiply(Arg1, 8), Local2)
	And(ShiftRight(GPRD(Arg0), Local2), 0xFFFFFF00, Local3)
	ShiftLeft (Or(And(Arg2, 0x000000FF),Local3), Local2, Local4)
	GPWR (Arg0, Local4)
}

/* Read pin control byte 0 */
Method (GPR0, 0x1)
{
	/* Arg0 - GPIO pin control MMIO address */
	Return (GPGB(Arg0, 0))
}

/* Read pin control byte 1 */
Method (GPR1, 0x1)
{
	/* Arg0 - GPIO pin control MMIO address */
	Return (GPGB(Arg0, 1))
}

/* Read pin control byte 2 */
Method (GPR2, 0x1)
{
	/* Arg0 - GPIO pin control MMIO address */
	Return (GPGB(Arg0, 2))
}

/* Read pin control byte 3 */
Method (GPR3, 0x1)
{
	Return (GPGB(Arg0, 3))
}

/* Write pin control byte 0 */
Method (GPW0, 0x2)
{
	/* Arg0 - GPIO pin control MMIO address */
	/* Arg1 - Value for control register */
	GPSB (Arg0, 0, Arg1)
}

/* Write pin control byte 1 */
Method (GPW1, 0x2)
{
	/* Arg0 - GPIO pin control MMIO address */
	/* Arg1 - Value for control register */
	GPSB (Arg0, 1, Arg1)
}

/* Write pin control byte 2 */
Method (GPW2, 0x2)
{
	/* Arg0 - GPIO pin control MMIO address */
	/* Arg1 - Value for control register */
	GPSB (Arg0, 2, Arg1)
}

/* Write pin control byte 3 */
Method (GPW3, 0x2)
{
	/* Arg0 - GPIO pin control MMIO address */
	/* Arg1 - Value for control register */
	GPSB (Arg0, 3, Arg1)
}
