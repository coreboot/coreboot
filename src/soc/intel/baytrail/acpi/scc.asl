/* SPDX-License-Identifier: GPL-2.0-only */

Device (EMMC)
{
	Name (_HID, "80860F14")
	Name (_CID, "PNP0D40")
	Name (_UID, 1)
	Name (_DDN, "eMMC Controller 4.5")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0x1000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive,,,)
		{
			SCC_EMMC_IRQ
		}
	})

	Method (_CRS)
	{
		CreateDwordField (^RBUF, ^BAR0._BAS, RBAS)
		RBAS = \C0B0
		Return (^RBUF)
	}

	Method (_STA)
	{
		If (\C0EN == 1) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	OperationRegion (KEYS, SystemMemory, C0B1, 0x100)
	Field (KEYS, DWordAcc, NoLock, WriteAsZeros)
	{
		Offset (0x84),
		PSAT, 32,
	}

	Method (_PS3)
	{
		PSAT |= 3
		PSAT |= 0
	}

	Method (_PS0)
	{
		And (PSAT, 0xfffffffc, PSAT)
		PSAT |= 0
	}

	Device (EM45)
	{
		/* Slot 0, Function 8 */
		Name (_ADR, 0x8)

		Method (_RMV, 0, NotSerialized)
		{
			Return (0)
		}
	}
}

Device (SDIO)
{
	Name (_HID, "INT33BB")
	Name (_CID, "PNP0D40")
	Name (_UID, 2)
	Name (_DDN, "SDIO Controller")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0x1000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive,,,)
		{
			SCC_SDIO_IRQ
		}
	})

	Method (_CRS)
	{
		CreateDwordField (^RBUF, ^BAR0._BAS, RBAS)
		RBAS = \C1B0
		Return (^RBUF)
	}

	Method (_STA)
	{
		If (\C1EN == 1) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	OperationRegion (KEYS, SystemMemory, C1B1, 0x100)
	Field (KEYS, DWordAcc, NoLock, WriteAsZeros)
	{
		Offset (0x84),
		PSAT, 32,
	}

	Method (_PS3)
	{
		PSAT |= 3
		PSAT |= 0
	}

	Method (_PS0)
	{
		And (PSAT, 0xfffffffc, PSAT)
		PSAT |= 0
	}
}

Device (SDCD)
{
	Name (_HID, "80860F16")
	Name (_CID, "PNP0D40")
	Name (_UID, 3)
	Name (_DDN, "SD Card Controller")

	Name (RBUF, ResourceTemplate()
	{
		Memory32Fixed (ReadWrite, 0, 0x1000, BAR0)
		Interrupt (ResourceConsumer, Level, ActiveLow, Exclusive,,,)
		{
			SCC_SD_IRQ
		}
	})

	Method (_CRS)
	{
		CreateDwordField (^RBUF, ^BAR0._BAS, RBAS)
		RBAS = \C2B0
		Return (^RBUF)
	}

	Method (_STA)
	{
		If (\C2EN == 1) {
			Return (0xF)
		} Else {
			Return (0x0)
		}
	}

	OperationRegion (KEYS, SystemMemory, C2B1, 0x100)
	Field (KEYS, DWordAcc, NoLock, WriteAsZeros)
	{
		Offset (0x84),
		PSAT, 32,
	}

	Method (_PS3)
	{
		PSAT |= 3
		PSAT |= 0
	}

	Method (_PS0)
	{
		And (PSAT, 0xfffffffc, PSAT)
		PSAT |= 0
	}
}
