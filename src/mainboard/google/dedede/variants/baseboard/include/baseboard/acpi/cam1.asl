/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C3)
{
	Name (STA1, Zero)

	PowerResource (RCPR, 0x00, 0x0000)
	{
		Method (_ON, 0, Serialized)  /* _ON_: Power On */
		{
			MCON(1, 1) /* Clock 1, 19.2MHz */
			/* Check if another sensor is ON */
			IF(!STA0)
			{
				/* Other sensor is OFF, so turn on power signals. */
				PON()
			}
			/* Assert Reset */
			CTXS(GPP_D12)
			Sleep(5)	/* 5 us */
			/* DeAssert Reset */
			STXS(GPP_D12)
			Sleep(5)	/* 5 us */
			STA1 = 1
		}

		Method (_OFF, 0, Serialized)  /* _OFF_: Power Off */
		{
			MCOF(1) /* Clock 1 */
			/* Assert Reset */
			CTXS(GPP_D12)
			IF(!STA0)
			{
				/* Other sensor is OFF, so turn off power signals. */
				POFF()
			}
			STA1 = 0
		}

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (STA1)
		}
	}

	Device(CAM1)
	{
		Name (_HID, "OVTI8856")  /* _HID: Hardware ID */

		Name (_UID, Zero)  /* _UID: Unique ID */

		Name (_DDN, "Ov 8856 Camera")  /* _DDN: DOS Device Name */

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()  /* _CRS: Current Resource Settings */
		{
			I2cSerialBus (0x0010, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C3",
				0x00, ResourceConsumer, ,
			)
		})

		Name (_PR0, Package (0x01)  /* _PR0: Power Resources for D0 */
		{
			RCPR
		})

		Name (_PR3, Package (0x01)  /* _PR3: Power Resources for D3hot */
		{
			RCPR
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
				}
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
			Package (0x05)
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
					Package (0x04)
					{
						One,
						0x02,
						0x03,
						0x04,
					}
				},

				Package (0x02)
				{
					"link-frequencies",
					Package (0x02)
					{
						0x15752A00,
						0xABA9500
					}
				},

				Package (0x02)
				{
					"remote-endpoint",
					Package (0x03)
					{
						IPU0,
						One,
						Zero
					}
				}
			}
		})
	}
}
