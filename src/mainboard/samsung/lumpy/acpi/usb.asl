/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.EHC1.HUB7.PRT1)
{
	// Hub Port 1
	Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
	{
		0xFF,	// Connectable
		0xFF,	// OEM Connector
		Zero,	// Reserved
		Zero	// Reserved
	})

	// Not Visible
	Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
	{
		Return (GPLD (Zero))
	}

	Device (USB1)
	{
		Name (_ADR, 1)

		// Left USB Port
		Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
		{
			0xFF,	// Connectable
			Zero,	// USB Port
			Zero,	// Reserved
			Zero	// Reserved
		})

		// Visible
		Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
		{
			Return (GPLD (1))
		}

	}

	Device (USB2)
	{
		Name (_ADR, 2)

		// Right USB Port
		Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
		{
			0xFF,	// Connectable
			Zero,	// USB Port
			Zero,	// Reserved
			Zero	// Reserved
		})

		// Visible
		Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
		{
			Return (GPLD (1))
		}
	}

	Device (USB4)
	{
		Name (_ADR, 4)

		// SD Card Slot
		Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
		{
			0xFF,	// Connectable
			0xFF,	// OEM Connector
			Zero,	// Reserved
			Zero	// Reserved
		})

		// Not Visible
		Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
		{
			Return (GPLD (Zero))
		}
	}

}

Scope (\_SB.PCI0.EHC2.HUB7.PRT1)
{
	// Hub Port 2
	Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
	{
		0xFF,	// Connectable
		0xFF,	// OEM Connector
		Zero,	// Reserved
		Zero	// Reserved
	})

	// Not Visible
	Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
	{
		Return (GPLD (Zero))
	}

	Device (USB4)
	{
		Name (_ADR, 4)

		// Webcam
		Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
		{
			0xFF,	// Connectable
			0xFF,	// OEM Connector
			Zero,	// Reserved
			Zero	// Reserved
		})

		// Not Visible
		Method (_PLD, 0, NotSerialized)  // _PLD: Physical Location of Device
		{
			Return (GPLD (Zero))
		}
	}
}
