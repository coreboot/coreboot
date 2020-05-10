/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C3)
{
	Name (STA0, Zero)

	/* Method to turn off Power Rails */
	Method (POFF, 0)
	{
		/* Disable PP1200 lane */
		CTXS(GPP_D14)
		/* Disable PP2800 lane */
		CTXS(GPP_D13)
	}

	Method (PON, 0)
	{
		/* Enable PP2800 lane */
		STXS(GPP_D13)
		/* Enable PP1200 lane */
		STXS(GPP_D14)
	}

	PowerResource (FCPR, 0x00, 0x0000)
	{
		Method (_ON, 0, Serialized)  /* _ON_: Power On */
		{
			MCON(0, 1) /* Clock 0, 19.2MHz */
			IF(!STA1)
			{
				/* Other sensor is OFF, so turn on power signals. */
				PON()
			}
			/* Assert Reset */
			CTXS(GPP_D15)
			Sleep(5)	/* 5 us */
			/* Deassert Reset */
			STXS(GPP_D15)
			Sleep(5)	/* 5 us */
			STA0 = 1
		}

		Method (_OFF, 0, Serialized)  /* _OFF_: Power Off */
		{
			MCOF(0) /* Clock 0 */
			/* Assert Reset */
			CTXS(GPP_D15)
			IF(!STA1)
			{
				/* Other sensor is OFF, so turn off power signals. */
				POFF()
			}
			STA0 = 0
		}

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (STA0)
		}
	}

	Device (CAM0)
	{
		Name (_HID, "OVTI9734")  /* _HID: Hardware ID */

		Name (_UID, Zero)  /* _UID: Unique ID */

		Name (_DDN, "Ov 9734 Camera")  /* _DDN: DOS Device Name */

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()  /* _CRS: Current Resource Settings */
		{
			I2cSerialBus (0x0036, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C3",
				0x00, ResourceConsumer, ,
			)
		})

		Name (_PR0, Package (0x01)  /* _PR0: Power Resources for D0 */
		{
			FCPR
		})

		Name (_PR3, Package (0x01)  /* _PR3: Power Resources for D3hot */
		{
			FCPR
		})

		Name (_DSD, Package (0x04)  /* _DSD: Device-Specific Data */
		{
			ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package (0x01)
			{
				Package (0x02)
				{
					"port0",
					"PRT0"
				}
			},

			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package (0x01)
			{
				Package (0x02)
				{
					"clock-frequency",
					0x0124F800
				},
			}
		})

		Name (PRT0, Package (0x04)
		{
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package (0x01)
			{
				Package (0x02)
				{
					"port",
					Zero
				}
			},

			ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package (0x01)
			{
				Package (0x02)
				{
					"endpoint0",
					"EP00"
				}
			}
		})

		Name (EP00, Package (0x02)
		{
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package (0x03)
			{
				Package (0x02)
				{
					"endpoint",
					Zero
				},

				Package (0x02)
				{
					"link-frequencies",
					Package (0x01)
					{
						0x325AA000
					}
				},

				Package (0x02)
				{
					"remote-endpoint",
					Package (0x03)
					{
						IPU0,
						Zero,
						Zero
					}
				}
			}
		})
	}
}
