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
 */

#include <soc/intel/fsp_baytrail/include/soc/iomap.h>
#include <soc/intel/fsp_baytrail/include/soc/irq.h>

/* SouthCluster GPIO */
Device (GPSC)
{
	Name (_HID, "INT33FC")
	Name (_CID, "INT33FC")
	Name (_UID, 1)

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0x1000, RMEM)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,,)
		{
			GPIO_SC_IRQ
		}
	})

	Method (_CRS)
	{
		CreateDwordField (^RBUF, ^RMEM._BAS, RBAS)
		Add (IO_BASE_ADDRESS, IO_BASE_OFFSET_GPSCORE, RBAS)
		Return (^RBUF)
	}

	Method (_STA)
	{
		Return (0xF)
	}
}

/* NorthCluster GPIO */
Device (GPNC)
{
	Name (_HID, "INT33FC")
	Name (_CID, "INT33FC")
	Name (_UID, 2)

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0x1000, RMEM)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,,)
		{
			GPIO_NC_IRQ
		}
	})

	Method (_CRS)
	{
		CreateDwordField (^RBUF, ^RMEM._BAS, RBAS)
		Add (IO_BASE_ADDRESS, IO_BASE_OFFSET_GPNCORE, RBAS)
		Return (^RBUF)
	}

	Method (_STA)
	{
		Return (0xF)
	}
}

/* SUS GPIO */
Device (GPSS)
{
	Name (_HID, "INT33FC")
	Name (_CID, "INT33FC")
	Name (_UID, 3)

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0x1000, RMEM)
		Interrupt (ResourceConsumer, Level, ActiveLow, Shared,,,)
		{
			GPIO_SUS_IRQ
		}
	})

	Method (_CRS)
	{
		CreateDwordField (^RBUF, ^RMEM._BAS, RBAS)
		Add (IO_BASE_ADDRESS, IO_BASE_OFFSET_GPSSUS, RBAS)
		Return (^RBUF)
	}

	Method (_STA)
	{
		Return (0xF)
	}
}
