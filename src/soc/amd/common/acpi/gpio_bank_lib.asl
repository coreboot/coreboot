/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/iomap.h>

#define GPIO_OUTPUT_SHIFT	22
#define GPIO_OUTPUT_VALUE	(1 << GPIO_OUTPUT_SHIFT)

/* Get pin control MMIO address */
Method (GPAD, 0x1)
{
	/* Arg0 - GPIO pin number */
	Return ((Arg0 * 4) + ACPIMMIO_GPIO0_BASE)
}

/* Read pin control dword */
Method (GPRD, 0x1, Serialized)
{
	/* Arg0 - GPIO pin control MMIO address */
	Local0 = Arg0
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
	Local0 = Arg0
	OperationRegion (GPDW, SystemMemory, Local0, 4)
	Field (GPDW, AnyAcc, NoLock, Preserve) {
		TEMP,32
	}
	TEMP = Arg1
}

Method (GPGB, 0x2)
{
	/*
	 * Returns the desired byte
	 * Arg0 - GPIO pin control MMIO address
	 * Arg1 - Desired byte (0 through 3)
	 */
	Local2 = Arg1 * 8
	Return ((GPRD (Arg0) >> Local2) & 0x000000FF)
}

Method (GPSB, 0x3)
{
	/*
	 * Reads dword, replace byte, write back dword
	 * Arg0 - GPIO pin control MMIO address
	 * Arg1 - Desired byte (0 through 3)
	 * Arg2 - Value
	 */
	Local2 = Arg1 * 8
	Local3 = (GPRD(Arg0) >> Local2) & 0xFFFFFF00
	Local4 = ((Arg2 & 0x000000FF) | Local3) << Local2
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

/*
 * Set GPIO Output Value
 * Arg0 - GPIO Number
 */
Method (STXS, 1, Serialized)
{
	OperationRegion (GPDW, SystemMemory, GPAD (Arg0), 4)
	Field (GPDW, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	VAL0 |= GPIO_OUTPUT_VALUE
}

/*
 * Clear GPIO Output Value
 * Arg0 - GPIO Number
 */
Method (CTXS, 1, Serialized)
{
	OperationRegion (GPDW, SystemMemory, GPAD (Arg0), 4)
	Field (GPDW, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	VAL0 &= ~GPIO_OUTPUT_VALUE
}
