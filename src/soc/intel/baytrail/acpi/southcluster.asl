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

Scope(\)
{
	// IO-Trap at 0x800. This is the ACPI->SMI communication interface.

	OperationRegion(IO_T, SystemIO, 0x800, 0x10)
	Field(IO_T, ByteAcc, NoLock, Preserve)
	{
		Offset(0x8),
		TRP0, 8		// IO-Trap at 0x808
	}
}

/* Device Resource Consumption */
Device (PDRC)
{
	Name (_HID, EISAID("PNP0C02"))
	Name (_UID, 1)

	Name (PDRS, ResourceTemplate() {
		Memory32Fixed(ReadWrite, ABORT_BASE_ADDRESS, ABORT_BASE_SIZE)
		Memory32Fixed(ReadWrite, MCFG_BASE_ADDRESS, MCFG_BASE_SIZE)
		Memory32Fixed(ReadWrite, PMC_BASE_ADDRESS, PMC_BASE_SIZE)
		Memory32Fixed(ReadWrite, IO_BASE_ADDRESS, IO_BASE_SIZE)
		Memory32Fixed(ReadWrite, ILB_BASE_ADDRESS, ILB_BASE_SIZE)
		Memory32Fixed(ReadWrite, SPI_BASE_ADDRESS, SPI_BASE_SIZE)
		Memory32Fixed(ReadWrite, MPHY_BASE_ADDRESS, MPHY_BASE_SIZE)
		Memory32Fixed(ReadWrite, PUNIT_BASE_ADDRESS, PUNIT_BASE_SIZE)
		Memory32Fixed(ReadWrite, RCBA_BASE_ADDRESS, RCBA_BASE_SIZE)
#if CONFIG_CHROMEOS_RAMOOPS
		Memory32Fixed(ReadWrite, CONFIG_CHROMEOS_RAMOOPS_RAM_START,
					 CONFIG_CHROMEOS_RAMOOPS_RAM_SIZE)
#endif
	})

	// Current Resource Settings
	Method (_CRS, 0, Serialized)
	{
		Return(PDRS)
	}
}
