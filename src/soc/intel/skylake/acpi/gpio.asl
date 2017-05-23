/*
 * This file is part of the coreboot project.
 *
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
 */
#include <soc/gpio_defs.h>

Device (GPIO)
{
	Name (_HID, "INT344B")
	Name (_UID, 1)
	Name (_DDN, "GPIO Controller")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0, COM0)
		Memory32Fixed (ReadWrite, 0, 0, COM1)
		Memory32Fixed (ReadWrite, 0, 0, COM3)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
			{ 0 }
	})

	Method (_CRS, 0, NotSerialized)
	{
		/* GPIO Community 0 */
		CreateDWordField (^RBUF, ^COM0._BAS, BAS0)
		CreateDWordField (^RBUF, ^COM0._LEN, LEN0)
		Store (^^PCRB (PID_GPIOCOM0), BAS0)
		Store (GPIO_BASE_SIZE, LEN0)

		/* GPIO Community 1 */
		CreateDWordField (^RBUF, ^COM1._BAS, BAS1)
		CreateDWordField (^RBUF, ^COM1._LEN, LEN1)
		Store (^^PCRB (PID_GPIOCOM1), BAS1)
		Store (GPIO_BASE_SIZE, LEN1)

		/* GPIO Community 3 */
		CreateDWordField (^RBUF, ^COM3._BAS, BAS3)
		CreateDWordField (^RBUF, ^COM3._LEN, LEN3)
		Store (^^PCRB (PID_GPIOCOM3), BAS3)
		Store (GPIO_BASE_SIZE, LEN3)

		CreateDWordField (^RBUF, ^GIRQ._INT, IRQN)
		And (^^PCRR (PID_GPIOCOM0, MISCCFG_OFFSET),
			GPIO_DRIVER_IRQ_ROUTE_MASK, Local0)

		If (LEqual (Local0, GPIO_DRIVER_IRQ_ROUTE_IRQ14)) {
			Store (GPIO_IRQ14, IRQN)
		} Else {
			Store (GPIO_IRQ15, IRQN)
		}

		Return (RBUF)
	}

	Method (_STA, 0, NotSerialized)
	{
		Return (0xF)
	}
}

/*
 * Get GPIO DW0 Address
 * Arg0 - GPIO Number
 */
Method (GADD, 1, NotSerialized)
{
#if IS_ENABLED(CONFIG_SKYLAKE_SOC_PCH_H)
	/* GPIO Community 0 */
	If (LAnd (LGreaterEqual (Arg0, GPP_A0), LLessEqual (Arg0, GPP_B23)))
	{
		Store (PID_GPIOCOM0, Local0)
		Subtract (Arg0, GPP_A0, Local1)
	}
	/* GPIO Community 1 */
	If (LAnd (LGreaterEqual (Arg0, GPP_C0), LLessEqual (Arg0, GPP_H23)))
	{
		Store (PID_GPIOCOM1, Local0)
		Subtract (Arg0, GPP_C0, Local1)
	}
	/* GPIO Community 03 */
	If (LAnd (LGreaterEqual (Arg0, GPP_I0), LLessEqual (Arg0, GPP_I10)))
	{
		Store (PID_GPIOCOM3, Local0)
		Subtract (Arg0, GPP_I0, Local1)
	}
#else
	/* GPIO Community 0 */
	If (LAnd (LGreaterEqual (Arg0, GPP_A0), LLessEqual (Arg0, GPP_B23)))
	{
		Store (PID_GPIOCOM0, Local0)
		Subtract (Arg0, GPP_A0, Local1)
	}
	/* GPIO Community 1 */
	If (LAnd (LGreaterEqual (Arg0, GPP_C0), LLessEqual (Arg0, GPP_E23)))
	{
		Store (PID_GPIOCOM1, Local0)
		Subtract (Arg0, GPP_C0, Local1)
	}
	/* GPIO Community 03*/
	If (LAnd (LGreaterEqual (Arg0, GPP_F0), LLessEqual (Arg0, GPP_G7)))
	{
		Store (PID_GPIOCOM3, Local0)
		Subtract (Arg0, GPP_F0, Local1)
	}
#endif /* CONFIG_SKYLAKE_SOC_PCH_H */
	Store (PCRB (Local0), Local2)
	Add (Local2, PAD_CFG_DW_OFFSET, Local2)
	Return (Add (Local2, Multiply (Local1, 8)))
}

/*
 * Get GPIO Rx Value
 * Arg0 - GPIO Number
 */
Method (GRXS, 1, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	And (GPIORXSTATE_MASK, ShiftRight (VAL0, GPIORXSTATE_SHIFT), Local0)

	Return (Local0)
}

/*
 * Get GPIO Tx Value
 * Arg0 - GPIO Number
 */
Method (GTXS, 1, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	And (GPIOTXSTATE_MASK, ShiftRight (VAL0, GPIOTXSTATE_SHIFT), Local0)

	Return (Local0)
}

/*
 * Set GPIO Tx Value
 * Arg0 - GPIO Number
 */
Method (STXS, 1, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	Or (GPIOTXSTATE_MASK, VAL0, VAL0)
}

/*
 * Clear GPIO Tx Value
 * Arg0 - GPIO Number
 */
Method (CTXS, 1, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	And (Not (GPIOTXSTATE_MASK), VAL0, VAL0)
}
