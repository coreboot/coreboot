/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.XHCI.HUB7.PRT2)
{
	// USB 2.0 Port 1
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
Scope (\_SB.PCI0.XHCI.HUB7.PRT3)
{
	// USB 2.0 Port 2
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
Scope (\_SB.PCI0.XHCI.HUB7.PRT4)
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
Scope (\_SB.PCI0.XHCI.HUB7.PRT5)
{
	// USB 2.0 Port 3
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
Scope (\_SB.PCI0.XHCI.HUB7.PRT6)
{
	// USB 2.0 Port 4
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
Scope (\_SB.PCI0.XHCI.HUB7.SSP1)
{
    // USB 3.0 Port 1
    Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		Zero,	// Reserved
		Zero	// Reserved
	})
}
Scope (\_SB.PCI0.XHCI.HUB7.SSP2)
{
    // USB 3.0 Port 2
    Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		Zero,	// Reserved
		Zero	// Reserved
	})
}
Scope (\_SB.PCI0.XHCI.HUB7.SSP3)
{
    // USB 3.0 Port 3
    Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		Zero,	// Reserved
		Zero	// Reserved
	})
}
Scope (\_SB.PCI0.XHCI.HUB7.SSP4)
{
    // USB 3.0 Port 4
    Name (_UPC, Package (0x04)  // _UPC: USB Port Capabilities
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		Zero,	// Reserved
		Zero	// Reserved
	})
}
