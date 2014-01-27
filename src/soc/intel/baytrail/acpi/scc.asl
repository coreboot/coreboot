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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

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
		Store (\C0B0, RBAS)
		Return (^RBUF)
	}

	Method (_STA)
	{
		If (LEqual (\C0EN, 1)) {
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
		Or (PSAT, 0x00000003, PSAT)
		Or (PSAT, 0x00000000, PSAT)
	}

	Method (_PS0)
	{
		And (PSAT, 0xfffffffc, PSAT)
		Or (PSAT, 0x00000000, PSAT)
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
		Store (\C1B0, RBAS)
		Return (^RBUF)
	}

	Method (_STA)
	{
		If (LEqual (\C1EN, 1)) {
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
		Or (PSAT, 0x00000003, PSAT)
		Or (PSAT, 0x00000000, PSAT)
	}

	Method (_PS0)
	{
		And (PSAT, 0xfffffffc, PSAT)
		Or (PSAT, 0x00000000, PSAT)
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
		Store (\C2B0, RBAS)
		Return (^RBUF)
	}

	Method (_STA)
	{
		If (LEqual (\C2EN, 1)) {
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
		Or (PSAT, 0x00000003, PSAT)
		Or (PSAT, 0x00000000, PSAT)
	}

	Method (_PS0)
	{
		And (PSAT, 0xfffffffc, PSAT)
		Or (PSAT, 0x00000000, PSAT)
	}
}
