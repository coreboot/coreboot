/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Intel Corp.
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


Device (GPIO)
{
	Name (_HID, "INT3455")
	Name (_UID, 0)
	Name (_DDN, "GPIO Controller")

	Method (_CRS, 0, NotSerialized)
	{
		Name (RBUF, ResourceTemplate()
		{
			Memory32Fixed (ReadWrite, 0, 0, COM0)
			Memory32Fixed (ReadWrite, 0, 0, COM1)
			Memory32Fixed (ReadWrite, 0, 0, COM2)
			Memory32Fixed (ReadWrite, 0, 0, COM4)
			Memory32Fixed (ReadWrite, 0, 0, COM5)
			Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,, GIRQ)
				{ GPIO_IRQ14 }
		})

		/* GPIO Community 0 */
		CreateDWordField (RBUF, COM0._BAS, BAS0)
		CreateDWordField (RBUF, COM0._LEN, LEN0)
		Store (PCRB (PID_GPIOCOM0), BAS0)
		Store (GPIO_BASE_SIZE, LEN0)

		/* GPIO Community 1 */
		CreateDWordField (RBUF, COM1._BAS, BAS1)
		CreateDWordField ( RBUF, COM1._LEN, LEN1)
		Store (PCRB (PID_GPIOCOM1), BAS1)
		Store (GPIO_BASE_SIZE, LEN1)

		/* GPIO Community 2 */
		CreateDWordField (RBUF, COM2._BAS, BAS2)
		CreateDWordField (RBUF, COM2._LEN, LEN2)
		Store (PCRB (PID_GPIOCOM2), BAS2)
		Store (GPIO_BASE_SIZE, LEN2)

		/* GPIO Community 4 */
		CreateDWordField (RBUF, COM4._BAS, BAS4)
		CreateDWordField (RBUF, COM4._LEN, LEN4)
		Store (PCRB (PID_GPIOCOM4), BAS4)
		Store (GPIO_BASE_SIZE, LEN4)

		/* GPIO Community 5 */
		CreateDWordField (RBUF, COM5._BAS, BAS5)
		CreateDWordField (RBUF, COM5._LEN, LEN5)
		Store (PCRB (PID_GPIOCOM5), BAS5)
		Store (GPIO_BASE_SIZE, LEN5)

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
	/* GPIO Community 0 */
	If (Arg0 >= GPP_G0 && Arg0 <= GPP_A23)
	{
		Local0 = PID_GPIOCOM0
		Subtract (Arg0, GPP_A0, Local1)
	}
	/* GPIO Community 1 */
	If (Arg0 >= GPP_H0 && Arg0 <= GPP_F19)
	{
		Local0 = PID_GPIOCOM1
		Subtract (Arg0, GPP_D0, Local1)
	}
	/* GPIO Community 2 */
	If (Arg0 >= GPD0 && Arg0 <= GPD11)
	{
		Local0 = PID_GPIOCOM2
		Subtract (Arg0, GPD0, Local1)
	}
	/* GPIO Community 4 */
	If (Arg0 >= GPP_C0 && Arg0 <= GPP_E23)
	{
		Local0 = PID_GPIOCOM4
		Subtract (Arg0, GPP_C0, Local1)
	}
	/* GPIO Community 05*/
	If (Arg0 >= GPP_R0 && Arg0 <= GPP_S7)
	{
		Local0 = PID_GPIOCOM5
		Subtract (Arg0, GPP_R0, Local1)
	}
	Store (PCRB (Local0), Local2)
	Add (Local2, PAD_CFG_BASE, Local2)
	Return (Add (Local2, Multiply (Local1, 16)))
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
	And (GPIORXSTATE_MASK, ShiftRight (VAL0, GPIORXSTATE_SHIFT), Local0)

	Return (Local0)
}
