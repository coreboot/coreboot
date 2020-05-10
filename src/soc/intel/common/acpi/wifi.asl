/* SPDX-License-Identifier: GPL-2.0-only */

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
