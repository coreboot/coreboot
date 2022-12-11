/* SPDX-License-Identifier: GPL-2.0-only */

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
		RBAS = IO_BASE_ADDRESS + COMMUNITY_OFFSET_GPSOUTHWEST
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
		RBAS = IO_BASE_ADDRESS + COMMUNITY_OFFSET_GPNORTH
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
		RBAS = IO_BASE_ADDRESS + COMMUNITY_OFFSET_GPEAST
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
		RBAS = IO_BASE_ADDRESS + COMMUNITY_OFFSET_GPSOUTHEAST
		Return (^RBUF)
	}

	Method (_STA)
	{
		Return (0xF)
	}
}
