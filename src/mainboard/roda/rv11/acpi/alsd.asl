/* SPDX-License-Identifier: GPL-2.0-only */

Device (ALSD)
{
	Name (_HID, "ACPI0008")  // _HID: Hardware ID
	Method (_STA, 0, NotSerialized)  // _STA: Status
	{
		Return (0x0F)
	}

	Method (_ALI, 0, NotSerialized)  // _ALI: Ambient Light Illuminance
	{
		Local0 = \_SB.PCI0.LPCB.EC0.LUXH
		Local0 = (Local0 << 8) | \_SB.PCI0.LPCB.EC0.LUXL
		Debug = "-----> _ALI: "
		Debug = Local0
		Debug = \_SB.PCI0.LPCB.EC0.LUXH
		Debug = \_SB.PCI0.LPCB.EC0.LUXL
		Return (Local0)
	}

	Name (_ALR, Package (0x05)  // _ALR: Ambient Light Response
	{
		Package (0x02)
		{
			0x46,
			Zero
		},

		Package (0x02)
		{
			0x49,
			0x0A
		},

		Package (0x02)
		{
			0x55,
			0x50
		},

		Package (0x02)
		{
			0x64,
			0x012C
		},

		Package (0x02)
		{
			0x96,
			0x03E8
		}
	})
}
