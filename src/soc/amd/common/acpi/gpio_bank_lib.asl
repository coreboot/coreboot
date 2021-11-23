/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio_map.h>
#include <amdblocks/gpio_defs.h>
#include <soc/iomap.h>

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

/*
 * Get GPIO Input Value
 * Arg0 - GPIO Number
 */
Method (GRXS, 1, Serialized)
{
	OperationRegion (GPDW, SystemMemory, GPAD (Arg0), 4)
	Field (GPDW, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	Local0 = (GPIO_PIN_STS & VAL0) >> GPIO_PIN_STS_SHIFT

	Return (Local0)
}

/*
 * Get GPIO Output Value
 * Arg0 - GPIO Number
 */
Method (GTXS, 1, Serialized)
{
	OperationRegion (GPDW, SystemMemory, GPAD (Arg0), 4)
	Field (GPDW, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	Local0 = (GPIO_OUTPUT_VALUE & VAL0) >> GPIO_OUTPUT_SHIFT

	Return (Local0)
}
