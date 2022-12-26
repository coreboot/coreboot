/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.XHCI.HUB7.PRT2)
{
	// Front Top USB 2.0
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0,	// USB Port
		0,	// Reserved
		0	// Reserved
	})

	// Visible
	Method (_PLD, 0, NotSerialized)
	{
		Return (GPLD (1))
	}
}
Scope (\_SB.PCI0.XHCI.HUB7.PRT3)
{
	// Front Bottom USB 2.0
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0,	// USB Port
		0,	// Reserved
		0	// Reserved
	})

	// Visible
	Method (_PLD, 0, NotSerialized)
	{
		Return (GPLD (1))
	}
}
Scope (\_SB.PCI0.XHCI.HUB7.PRT4)
{
	// Bluetooth
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0xFF,	// OEM Connector
		0,	// Reserved
		0	// Reserved
	})

	// Not Visible
	Method (_PLD, 0, NotSerialized)
	{
		Return (GPLD (0))
	}
}
Scope (\_SB.PCI0.XHCI.HUB7.PRT5)
{
	// Back Right USB 2.0
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0,	// USB Port
		0,	// Reserved
		0	// Reserved
	})

	// Visible
	Method (_PLD, 0, NotSerialized)
	{
		Return (GPLD (1))
	}
}
Scope (\_SB.PCI0.XHCI.HUB7.PRT6)
{
	// Back Left USB 2.0
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0,	// USB Port
		0,	// Reserved
		0	// Reserved
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
	// Connected, OEM Connector, Reserved, Reserved
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0xFF,	// OEM Connector
		0,	// Reserved
		0	// Reserved
	})

	// Not Visible
	Method (_PLD, 0, NotSerialized)
	{
		Return (GPLD (0))
	}
}
Scope (\_SB.PCI0.XHCI.HUB7.SSP3)
{
	// Front Top USB 3.0
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		0,	// Reserved
		0	// Reserved
	})
}
Scope (\_SB.PCI0.XHCI.HUB7.SSP4)
{
	// Front Bottom USB 3.0
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		0,	// Reserved
		0	// Reserved
	})
}
Scope (\_SB.PCI0.XHCI.HUB7.SSP5)
{
	// Back Right USB 3.0
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		0,	// Reserved
		0	// Reserved
	})
}
Scope (\_SB.PCI0.XHCI.HUB7.SSP6)
{
	// Back Left USB 3.0
	Name (_UPC, Package (0x04)
	{
		0xFF,	// Connectable
		0x03,	// USB 3.0 Port
		0,	// Reserved
		0	// Reserved
	})
}
