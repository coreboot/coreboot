/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.EHC1.HUB7.URMH)
{
	Scope (PRT0)
	{
		// Left USB Port
		Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
		{
			0xFF,	// Connectable
			0,	// USB Port
			0,	// Reserved
			0	// Reserved
		})

		// Visible
		Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
		{
			Return (GPLD (1))
		}

	}

	Scope (PRT1)
	{
		// Right USB Port
		Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
		{
			0xFF,	// Connectable
			0,	// USB Port
			0,	// Reserved
			0	// Reserved
		})

		// Visible
		Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
		{
			Return (GPLD (1))
		}
	}

	Scope (PRT3)
	{
		// SD Card Slot
		Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
		{
			0xFF,	// Connectable
			0xFF,	// OEM Connector
			0,	// Reserved
			0	// Reserved
		})

		// Not Visible
		Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
		{
			Return (GPLD (0))
		}
	}

}

Scope (\_SB.PCI0.EHC2.HUB7.URMH)
{
	Scope (PRT3)
	{
		// Webcam
		Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
		{
			0xFF,	// Connectable
			0xFF,	// OEM Connector
			0,	// Reserved
			0	// Reserved
		})

		// Not Visible
		Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
		{
			Return (GPLD (0))
		}
	}
}
