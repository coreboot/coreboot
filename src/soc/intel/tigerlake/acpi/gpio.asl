/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2020 Intel Corp.
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
#include <soc/irq.h>
#include <soc/pcr_ids.h>

Device (GCM0)
{
	Name (_HID, "INT34C5")
	Name (_UID, 0)
	Name (_DDN, "GPIO Controller Community 0")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, GPIO_BASE_SIZE, COM0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
			{ GPIO_IRQ14 }
	})
	Method (_CRS, 0, NotSerialized)
	{
		CreateDWordField (^RBUF, ^COM0._BAS, BAS0)
		BAS0 = ^^PCRB (PID_GPIOCOM0)
		Return (^RBUF)
	}
	Method (_STA)
	{
		Return (0xF)
	}
}

Device (GCM1)
{
	Name (_HID, "INT34C5")
	Name (_UID, 1)
	Name (_DDN, "GPIO Controller Community 1")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, GPIO_BASE_SIZE, COM1)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
			{ GPIO_IRQ14 }
	})
	Method (_CRS, 0, NotSerialized)
	{
		CreateDWordField (^RBUF, ^COM1._BAS, BAS1)
		BAS1 = ^^PCRB (PID_GPIOCOM1)
		Return (^RBUF)
	}
	Method (_STA)
	{
		Return (0xF)
	}
}

Device (GCM4)
{
	Name (_HID, "INT34C5")
	Name (_UID, 4)
	Name (_DDN, "GPIO Controller Community 4")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, GPIO_BASE_SIZE, COM4)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
			{ GPIO_IRQ14 }
	})
	Method (_CRS, 0, NotSerialized)
	{
		CreateDWordField (^RBUF, ^COM4._BAS, BAS4)
		BAS4 = ^^PCRB (PID_GPIOCOM4)
		Return (^RBUF)
	}
	Method (_STA)
	{
		Return (0xF)
	}
}

Device (GCM5)
{
	Name (_HID, "INT34C5")
	Name (_UID, 5)
	Name (_DDN, "GPIO Controller Community 5")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, GPIO_BASE_SIZE, COM5)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
		{ GPIO_IRQ14 }
	})
	Method (_CRS, 0, NotSerialized)
	{
		CreateDWordField (^RBUF, ^COM5._BAS, BAS5)
		BAS5 = ^^PCRB (PID_GPIOCOM5)
		Return (^RBUF)
	}
	Method (_STA)
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
	/* GPIO Community 0 */
	If (Arg0 >= GPP_B0 && Arg0 <= GPP_A24)
	{
		Local0 = PID_GPIOCOM0
		Local1 = Arg0 - GPP_B0
	}
	/* GPIO Community 1 */
	If (Arg0 >= GPP_S0 && Arg0 <= vI2S2_RXD)
	{
		Local0 = PID_GPIOCOM1
		Local1 = Arg0 - GPP_S0
	}
	/* GPIO Community 2 */
	If (Arg0 >= GPD0 && Arg0 <= GPD_DRAM_RESETB)
	{
		Local0 = PID_GPIOCOM2
		Local1 = Arg0 - GPD0
	}
	/* GPIO Community 4 */
	If (Arg0 >= GPP_C0 && Arg0 <= GPP_DBG_PMODE)
	{
		Local0 = PID_GPIOCOM4
		Local1 = Arg0 - GPP_C0
	}
	/* GPIO Community 5 */
	If (Arg0 >= GPP_R0 && Arg0 <= GPP_CLK_LOOPBK)
	{
		Local0 = PID_GPIOCOM5
		Local1 = Arg0 - GPP_R0
	}
	Local2 = PCRB(Local0) + PAD_CFG_BASE + (Local1 * 16)
	Return (Local2)
}

/*
 * Get GPIO Value
 * Arg0 - GPIO Number
 */
Method (GRXS, 1, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	Local0 = GPIORXSTATE_MASK & (VAL0 >> GPIORXSTATE_SHIFT)

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
	Local0 = GPIOTXSTATE_MASK & VAL0

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
	VAL0 |= GPIOTXSTATE_MASK
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
	VAL0 &= ~GPIOTXSTATE_MASK
}
