/*
 * This file is part of the coreboot project.
 *
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

Device (WIFI)
{
	Name (_ADR, Zero)
	OperationRegion(WIXX, PCI_Config, 0x00, 0x10)
	Name (WRDX, Package()
	{
		// Revision
		0,
		Package()
		{
			// DomainType, 0x7:WiFi
			0x00000007,
			// Default Regulatory Domain Country identifier
			0x4150,
		}
	})
	Method(WRDD,0,Serialized)
	{
		Store(\CID1,Index (DeRefOf (Index (WRDX, 1)), 1)) // Country identifier

		Return(WRDX)
	}

}
