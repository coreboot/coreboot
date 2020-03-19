/* SPDX-License-Identifier: GPL-2.0-only */

Scope (_SB.PCI0.IPU0)
{
	Name (EP10, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x04)
		{
			Package (0x02)
			{
				"endpoint",
				Zero
			},

			Package (0x02)
			{
				"clock-lanes",
				Zero
			},

			Package (0x02)
			{
				"data-lanes",
				Package (0x02)
				{
					One,
					0x02,
				}
			},

			Package (0x02)
			{
				"remote-endpoint",
				Package (0x03)
				{
					^I2C5.CAM1,
					Zero,
					Zero
				}
			}
		}
	})
}
