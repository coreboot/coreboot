/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.XHCI.RHUB.PRT1)
{
	// Left USB 2.0
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
Scope (\_SB.PCI0.XHCI.RHUB.PRT2)
{
	// USB HUB
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
		Name (_ADR, 4)

		// Right USB 2.0
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

	Device (USB3)
	{
		Name (_ADR, 5)

		// SIM Card Slot
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
Scope (\_SB.PCI0.XHCI.RHUB.PRT3)
{
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
Scope (\_SB.PCI0.XHCI.RHUB.PRT4)
{
	// Bluetooth
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
Scope (\_SB.PCI0.XHCI.RHUB.SSP1)
{
	// Left USB 3.0
	Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		Zero,	// Reserved
		Zero	// Reserved
	})
}
