/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

#include <soc/iomap.h>
#include <soc/irq.h>

/* GPIO SouthWest Community */
Device (GPSW)
{
	Name (_HID, "INT33FF")
	Name (_CID, "INT33FF")
	Name (_UID, 1)

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0x8000, RMEM)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,,)
		{
			GPIO_SW_IRQ
		}
	})

	Method (_CRS)
	{
		CreateDwordField (^RBUF, ^RMEM._BAS, RBAS)
		Add (IO_BASE_ADDRESS, COMMUNITY_OFFSET_GPSOUTHWEST, RBAS)
		Return (^RBUF)
	}

	Method (_STA)
	{
		Return (0xF)
	}
}

/* GPIO North Community */
Device (GPNC)
{
	Name (_HID, "INT33FF")
	Name (_CID, "INT33FF")
	Name (_UID, 2)

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0x8000, RMEM)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,,)
		{
			GPIO_N_IRQ
		}
	})

	Method (_CRS)
	{
		CreateDwordField (^RBUF, ^RMEM._BAS, RBAS)
		Add (IO_BASE_ADDRESS, COMMUNITY_OFFSET_GPNORTH, RBAS)
		Return (^RBUF)
	}

	Method (_STA)
	{
		Return (0xF)
	}
}

/* GPIO East Community */
Device (GPEC)
{
	Name (_HID, "INT33FF")
	Name (_CID, "INT33FF")
	Name (_UID, 3)

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0x8000, RMEM)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,,)
		{
			GPIO_E_IRQ
		}
	})

	Method (_CRS)
	{
		CreateDwordField (^RBUF, ^RMEM._BAS, RBAS)
		Add (IO_BASE_ADDRESS, COMMUNITY_OFFSET_GPEAST, RBAS)
		Return (^RBUF)
	}

	Method (_STA)
	{
		Return (0xF)
	}
}

/* GPIO SouthEast Community */
Device (GPSE)
{
	Name (_HID, "INT33FF")
	Name (_CID, "INT33FF")
	Name (_UID, 4)

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0x8000, RMEM)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,,)
		{
			GPIO_SE_IRQ
		}
	})

	Method (_CRS)
	{
		CreateDwordField (^RBUF, ^RMEM._BAS, RBAS)
		Add (IO_BASE_ADDRESS, COMMUNITY_OFFSET_GPSOUTHEAST, RBAS)
		Return (^RBUF)
	}

	Method (_STA)
	{
		Return (0xF)
	}
}
