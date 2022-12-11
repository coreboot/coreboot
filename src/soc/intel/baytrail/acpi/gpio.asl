/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/iomap.h>
#include <soc/irq.h>

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
		RBAS = IO_BASE_ADDRESS + IO_BASE_OFFSET_GPSCORE
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
		RBAS = IO_BASE_ADDRESS + IO_BASE_OFFSET_GPNCORE
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
		RBAS = IO_BASE_ADDRESS + IO_BASE_OFFSET_GPSSUS
		Return (^RBUF)
	}

	Method (_STA)
	{
		Return (0xF)
	}
}
