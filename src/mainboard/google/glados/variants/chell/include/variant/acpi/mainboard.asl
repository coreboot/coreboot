/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.XHCI.RHUB.HS01)
{
	// Left Rear USB 2.0 Type-C
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
Scope (\_SB.PCI0.XHCI.RHUB.HS02)
{
	// Left Rear USB 2.0 Type-C
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
Scope (\_SB.PCI0.XHCI.RHUB.HS03)
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
Scope (\_SB.PCI0.XHCI.RHUB.HS04)
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
Scope (\_SB.PCI0.XHCI.RHUB.HS05)
{
	// Left USB 2.0 Type-A
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
Scope (\_SB.PCI0.XHCI.RHUB.HS07)
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
Scope (\_SB.PCI0.XHCI.RHUB.SS01)
{
	// Left Rear USB 3.0 Type-C
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		Zero,	// Reserved
		Zero	// Reserved
	})
}
Scope (\_SB.PCI0.XHCI.RHUB.SS02)
{
	// Left Front USB 3.0 Type-C
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		Zero,	// Reserved
		Zero	// Reserved
	})
}
Scope (\_SB.PCI0.XHCI.RHUB.SS03)
{
	// Left USB 3.0 Type-A
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		Zero,	// Reserved
		Zero	// Reserved
	})
}
Scope (\_SB.PCI0.XHCI.RHUB.SS04)
{
	// SD Card
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0
		Zero,	// Reserved
		Zero	// Reserved
	})
}
