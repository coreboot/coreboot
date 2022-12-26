/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.XHCI.HUB7.PRT1)
{
	// WWAN M.2 Slot
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0xFF,	// OEM Connector
		Zero,	// Reserved
		Zero	// Reserved
	})

	// Not Visible
	Method (_PLD, 0, NotSerialized)
	{
		Return (GPLD (Zero))
	}
}
Scope (\_SB.PCI0.XHCI.HUB7.PRT2)
{
	// Left USB 2.0
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		Zero,	// USB Port
		Zero,	// Reserved
		Zero	// Reserved
	})

	// Visible
	Method (_PLD, 0, NotSerialized)
	{
		Return (GPLD (1))
	}
}
Scope (\_SB.PCI0.XHCI.HUB7.PRT3)
{
	// Webcam
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0xFF,	// OEM Connector
		Zero,	// Reserved
		Zero	// Reserved
	})

	// Not Visible
	Method (_PLD, 0, NotSerialized)
	{
		Return (GPLD (Zero))
	}
}
Scope (\_SB.PCI0.XHCI.HUB7.PRT4)
{
	// Bluetooth
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0xFF,	// OEM Connector
		Zero,	// Reserved
		Zero	// Reserved
	})

	// Not Visible
	Method (_PLD, 0, NotSerialized)
	{
		Return (GPLD (Zero))
	}
}
Scope (\_SB.PCI0.XHCI.HUB7.PRT5)
{
	// Right USB 2.0
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		Zero,	// USB Port
		Zero,	// Reserved
		Zero	// Reserved
	})

	// Visible
	Method (_PLD, 0, NotSerialized)
	{
		Return (GPLD (1))
	}
}
Scope (\_SB.PCI0.XHCI.HUB7.PRT7)
{
	// SD Card
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0xFF,	// OEM Connector
		Zero,	// Reserved
		Zero	// Reserved
	})

	// Not Visible
	Method (_PLD, 0, NotSerialized)
	{
		Return (GPLD (Zero))
	}
}
Scope (\_SB.PCI0.XHCI.HUB7.SSP1)
{
	// Left USB 3.0
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		Zero,	// Reserved
		Zero	// Reserved
	})
}
