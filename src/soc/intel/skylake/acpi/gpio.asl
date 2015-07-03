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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <soc/irq.h>
#include <soc/iomap.h>
#include <soc/pcr.h>
#include <soc/gpio_defs.h>

/* PCR Register Access Methods PCR Dword Read arg0: PID arg1: Offset */
Method (PCRR, 2, Serialized)
{
	Add (ShiftLeft (Arg0, PCR_PORTID_SHIFT), Arg1, Local0)
	Add (PCH_PCR_BASE_ADDRESS, Local0, Local0)
	OperationRegion (PCR0, SystemMemory, Local0, 0x4)

	Field(PCR0, DWordAcc, Lock, Preserve)
	{
		Offset(0x00),
		DAT0, 32
	}
	Return (DAT0)
}

Device (GPIO)
{
	/* GPIO Controller */
	Name (_HID, "INT344B")
	Name (_UID, 1)

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, GPIO_BASE_SIZE, _R0)
		Memory32Fixed (ReadWrite, 0, GPIO_BASE_SIZE, _R1)
		Memory32Fixed (ReadWrite, 0, GPIO_BASE_SIZE, _R3)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared, ,, _R4)
		{
			GPIO_IRQ14,
		}
	})

	Method (_CRS, 0, NotSerialized)  /* _CRS: Current Resource Settings */
	{
		CreateDWordField (^RBUF, ^_R0._BAS, COM0)
		CreateDWordField (^RBUF, ^_R1._BAS, COM1)
		CreateDWordField (^RBUF, ^_R3._BAS, COM3)
		CreateDWordField (^RBUF, ^_R4._INT, IRQN)

		Store (Add (PCH_PCR_BASE_ADDRESS,
			ShiftLeft (PID_GPIOCOM0, PCR_PORTID_SHIFT)), COM0)
		Store (Add (PCH_PCR_BASE_ADDRESS,
			ShiftLeft (PID_GPIOCOM1, PCR_PORTID_SHIFT)), COM1)
		Store (Add (PCH_PCR_BASE_ADDRESS,
			ShiftLeft (PID_GPIOCOM3, PCR_PORTID_SHIFT)), COM3)
		Store (And (PCRR (PID_GPIOCOM0, MISCCFG_OFFSET),
			GPIO_DRIVER_IRQ_ROUTE_MASK), Local0)

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
