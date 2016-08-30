/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <soc/gpio_defs.h>

scope (\_SB) {

	Device (GPO0)
	{
		Name (_ADR, 0)
		Name (_HID, "INT3452")
		Name (_CID, "INT3452")
		Name (_DDN, "General Purpose Input/Output (GPIO) Controller - North" )
		Name (_UID, 1)

		Name (RBUF, ResourceTemplate ()
		{
			Memory32Fixed (ReadWrite, 0, 0x4000, RMEM)
			Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , )
			{
				GPIO_BANK_INT
			}
		})

		Method (_CRS, 0x0, NotSerialized)
		{
			CreateDwordField (^RBUF, ^RMEM._BAS, RBAS)
			ShiftLeft (GPIO_N, 16, Local0)
			Or (CONFIG_IOSF_BASE_ADDRESS, Local0, RBAS)
			Return (^RBUF)
		}

		Method (_STA, 0x0, NotSerialized)
		{
			Return(0xf)
		}
	}

	Device (GPO1)
	{
		Name (_ADR, 0)
		Name (_HID, "INT3452")
		Name (_CID, "INT3452")
		Name (_DDN, "General Purpose Input/Output (GPIO) Controller - Northwest" )
		Name (_UID, 2)

		Name (RBUF, ResourceTemplate ()
		{
			Memory32Fixed (ReadWrite, 0, 0x4000, RMEM)
			Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , )
			{
				GPIO_BANK_INT
			}
		})

		Method (_CRS, 0x0, NotSerialized)
		{
			CreateDwordField (^RBUF, ^RMEM._BAS, RBAS)
			ShiftLeft (GPIO_NW, 16, Local0)
			Or (CONFIG_IOSF_BASE_ADDRESS, Local0, RBAS)
			Return (^RBUF)
		}

		Method (_STA, 0x0, NotSerialized)
		{
			Return(0xf)
		}
	}

	Device (GPO2)
	{
		Name (_ADR, 0)
		Name (_HID, "INT3452")
		Name (_CID, "INT3452")
		Name (_DDN, "General Purpose Input/Output (GPIO) Controller - West" )
		Name (_UID, 3)

		Name (RBUF, ResourceTemplate ()
		{
			Memory32Fixed (ReadWrite, 0, 0x4000, RMEM)
			Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , )
			{
				GPIO_BANK_INT
			}
		})

		Method (_CRS, 0x0, NotSerialized)
		{
			CreateDwordField (^RBUF, ^RMEM._BAS, RBAS)
			ShiftLeft (GPIO_W, 16, Local0)
			Or (CONFIG_IOSF_BASE_ADDRESS, Local0, RBAS)
			Return (^RBUF)
		}

		Method (_STA, 0x0, NotSerialized)
		{
			Return(0xf)
		}
	}

	Device (GPO3)
	{
		Name (_ADR, 0)
		Name (_HID, "INT3452")
		Name (_CID, "INT3452")
		Name (_DDN, "General Purpose Input/Output (GPIO) Controller - Southwest" )
		Name (_UID, 4)

		Name (RBUF, ResourceTemplate ()
		{
			Memory32Fixed (ReadWrite, 0, 0x4000, RMEM)
			Interrupt (ResourceConsumer, Level, ActiveLow, Shared, , , )
			{
				GPIO_BANK_INT
			}
		})

		Method (_CRS, 0x0, NotSerialized)
		{
			CreateDwordField (^RBUF, ^RMEM._BAS, RBAS)
			ShiftLeft (GPIO_SW, 16, Local0)
			Or (CONFIG_IOSF_BASE_ADDRESS, Local0, RBAS)
			Return (^RBUF)
		}

		Method (_STA, 0x0, NotSerialized)
		{
			Return(0xf)
		}
	}
}

Scope(\_GPE)
{
	/* Dummy method for the Tier 1 GPIO SCI enable bit. When kernel reads
	 * _L0F in scope GPE it sets bit for gpio_tier1_sci_en in ACPI enable
	 * register at 0x430. For APL ACPI enable register DW0 i.e., ACPI
	 * GPE0a_EN at 0x430 is reserved.
	 */
	Method(_L0F, 0) {}
}
