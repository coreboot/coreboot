/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include "acpi/superio.asl"
#include "../gpio.h"

Scope (\_SB)
{
	Device (LID0)
	{
		Name (_HID, EisaId ("PNP0C0D"))
		Method (_LID, 0)
		{
			Return (\_SB.PCI0.LPCB.EC0.LIDS)
		}
	}

	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))
	}
}

Scope (\_SB.PCI0.I2C4)
{
	/* Standard Mode: HCNT, LCNT, SDA Hold Register */
	/* SDA Hold register value of 40 indicates
	 * sda hold time of 0.3us for ic_clk of 133MHz
	 */
	Name (SSCN, Package () { 0, 0, 40 })

	/* Fast Mode: HCNT, LCNT, SDA Hold Register */
	/* SDA Hold register value of 40 indicates
	 * sda hold time of 0.3us for ic_clk of 133MHz
	 */
	Name (FMCN, Package () { 0, 0, 40 })

	Device (ETPA)
	{
		Name (_HID, "ELAN0000")
		Name (_DDN, "Elan Touchpad")
		Name (_UID, 1)
		Name (ISTP, 1) /* Touchpad */

		Name (_CRS, ResourceTemplate()
		{
			I2cSerialBus (
				0x15,                     // SlaveAddress
				ControllerInitiated,      // SlaveMode
				400000,                   // ConnectionSpeed
				AddressingMode7Bit,       // AddressingMode
				"\\_SB.PCI0.I2C4",        // ResourceSource
			)
			Interrupt (ResourceConsumer, Edge, ActiveLow)
			{
				TOUCHPAD_INT
			}
		})

		Method (_STA)
		{
			Return (0xF)
		}
	}
}

/*
 * LPC Trusted Platform Module
 */
Scope (\_SB.PCI0.LPCB)
{
	#include <drivers/pc80/tpm/acpi/tpm.asl>
	#include "ec.asl"
}
