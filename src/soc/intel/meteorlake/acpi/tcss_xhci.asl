/* SPDX-License-Identifier: GPL-2.0-or-later */

OperationRegion (XPRT, SystemMemory, BASE(_ADR), 0x100)
Field (XPRT, ByteAcc, NoLock, Preserve)
{
	VDID,  32,
	Offset(0x74),   /* 0x74, XHCI CFG Power Control And Status */
	D0D3,  2,       /* 0x74 BIT[1:0] */
	    ,  6,
	PMEE,  1,       /* PME Enable */
	    ,  6,
	PMES,  1,       /* PME Status */
}

Method (_PS0, 0, Serialized)
{
	If (\_SB.PCI0.TXHC.PMEE == 1) {
		/* Clear PME_EN of CPU xHCI */
		\_SB.PCI0.TXHC.PMEE = 0
	}
}

Method (_PS3, 0, Serialized)
{
	If (\_SB.PCI0.TXHC.PMEE == 0) {
		/* Set PME_EN of CPU xHCI */
		\_SB.PCI0.TXHC.PMEE = 1
	}
}

Method (_S0W, 0x0, NotSerialized)
{
	Return (0x4)
}

/*
 * Variable to skip TCSS/TBT D3 cold; 1+: Skip D3CE, 0 - Enable D3CE
 * TCSS D3 Cold and TBT RTD3 is only available when system power state is in S0.
 */
Name (SD3C, 0)

Method (_PR0)
{
	Return (Package () { \_SB.PCI0.D3C })
}

Method (_PR3)
{
	Return (Package () { \_SB.PCI0.D3C })
}

/*
 * XHCI controller _DSM method
 */
Method (_DSM, 4, serialized)
{
	Return (Buffer() { 0 })
}

/*
 * _SXD and _SXW methods
 */
Method (_S3D, 0, NotSerialized)
{
	Return (3)
}

Method (_S4D, 0, NotSerialized)
{
	Return (3)
}

Method (_S3W, 0, NotSerialized)
{
	Return (3)
}

Method (_S4W, 0, NotSerialized)
{
	Return (3)
}

/*
 * Power resource for wake
 */
Method (_PRW, 0)
{
	Return (Package() { 0x6D, 4 })
}

/*
 * Device sleep wake
 */
Method (_DSW, 3)
{
	C2PM (Arg0, Arg1, Arg2, DCPM)
	/* If entering Sx (Arg1 > 1), need to skip TCSS D3Cold & TBT RTD3/D3Cold. */
	SD3C = Arg1
}

/*
 * xHCI Root Hub Device
 */
Device (RHUB)
{
	Name (_ADR, 0)

	/* High Speed Ports */
	Device (HS01)
	{
		Name (_ADR, 0x01)
	}

	/* Super Speed Ports */
	Device (SS01)
	{
		Name (_ADR, 0x02)
		Method (_DSD, 0, NotSerialized)
		{
			Return( Package ()
			{
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package ()
				{
					Package (2) { "usb4-host-interface", \_SB.PCI0.TDM0 },
					Package (2) { "usb4-port-number", 0 }
				}
			})
		}
	}

	Device (SS02)
	{
		Name (_ADR, 0x03)
		Method (_DSD, 0, NotSerialized)
		{
			Return( Package ()
			{
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package ()
				{
					Package (2) { "usb4-host-interface", \_SB.PCI0.TDM0 },
					Package (2) { "usb4-port-number", 1 }
				}
			})
		}
	}

	Device (SS03)
	{
		Name (_ADR, 0x04)
		Method (_DSD, 0, NotSerialized)
		{
			Return( Package ()
			{
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package ()
				{
					Package (2) { "usb4-host-interface", \_SB.PCI0.TDM1 },
					Package (2) { "usb4-port-number", 0 }
				}
			})
		}
	}

	Device (SS04)
	{
		Name (_ADR, 0x05)
		Method (_DSD, 0, NotSerialized)
		{
			Return( Package ()
			{
				ToUUID("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
				Package ()
				{
					Package (2) { "usb4-host-interface", \_SB.PCI0.TDM1 },
					Package (2) { "usb4-port-number", 1 }
				}
			})
		}
	}
}
