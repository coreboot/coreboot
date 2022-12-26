/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.IPU0)
{
	Name (_DSD, Package (0x02)  /* _DSD: Device-Specific Data */
	{
		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package (0x02)
		{
			Package (0x02)
			{
				"port0",
				"PRT0"
			},
			Package (0x02)
			{
				"port1",
				"PRT1"
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
				5
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

	Name (PRT1, Package (0x04)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x01)
		{
			Package (0x02)
			{
				"port",
				1
			}
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package (0x01)
		{
			Package (0x02)
			{
				"endpoint0",
				"EP10"
			}
		}
	})
}

Scope (\_SB.PCI0.IPU0)
{
	Name (EP10, Package (0x02)
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package (0x04)
		{
			Package (0x02)
			{
				"endpoint",
				0
			},
			Package (0x02)
			{
				"clock-lanes",
				0
			},
			Package (0x02)
			{
				"data-lanes",
				Package (0x02)
				{
					1,
					0x02
				}
			},
			Package (0x02)
			{
				"remote-endpoint",
				Package (0x03)
				{
					^I2C2.CAM1,
					0,
					0
				}
			}
		}
	})
}

Scope (\_SB.PCI0.I2C2)
{
	Name (STA, 0)
	PowerResource (FCPR, 0x00, 0x0000)
	{
		Method (_ON, 0, Serialized)  /* Front camera_ON_: Power On */
		{
			If (STA == 0)
			{
				/* Enable IMG_CLK */
				MCON(2,1) /* Clock 2, 19.2MHz */

				/* Pull RST low */
				CTXS(GPP_D4)

				/* Pull PWREN high */
				STXS(GPP_D17)
				Sleep(10) /* t9 */

				/* Pull RST high */
				STXS(GPP_D4)
				Sleep(1) /* t2 */

				STA = 1
			}
		}
		Method (_OFF, 0, Serialized)  /* Front camera_OFF_: Power Off */
		{
			If (STA == 1)
			{
				/* Disable IMG_CLK */
				Sleep(1) /* t0+t1 */
				MCOF(2) /* Clock 2 */

				/* Pull RST low */
				CTXS(GPP_D4)

				/* Pull PWREN low */
				CTXS(GPP_D17)

				STA = 0
			}
		}
		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (STA)
		}
	}

	Device (CAM1)
	{
		Name (_HID, "INT3474")  /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "Ov 2740 Camera")  /* _DDN: DOS Device Name */
		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}
		Name (_CRS, ResourceTemplate ()  /* _CRS: Current Resource Settings */
		{
			I2cSerialBus (0x0036, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
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
			Package (0x02)
			{
				Package (0x02)
				{
					"clock-frequency",
					0x0124F800
				},
				Package (0x02)
				{
					"i2c-allow-low-power-probe",
					0x01
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
					0
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
					0
				},
				Package (0x02)
				{
					"clock-lanes",
					0
				},
				Package (0x02)
				{
					"data-lanes",
					Package (0x02)
					{
						1,
						0x02
					}
				},
				Package (0x02)
				{
					"link-frequencies",
					Package (0x01)
					{
						0x15752A00
					}
				},
				Package (0x02)
				{
					"remote-endpoint",
					Package (0x03)
					{
						IPU0,
						1,
						0
					}
				}
			}
		})
	}
	Device (NVM0)
	{
		Name (_HID, "PRP0001")  // _HID: Hardware ID
		Name (_UID, 0x01)  // _UID: Unique ID
		Name (_DDN, "AT24 EEPROM")  // _DDN: DOS Device Name
		Method (_STA, 0, NotSerialized)  // _STA: Status
		{
			Return (0x0F)
		}
		Name (_CRS, ResourceTemplate ()  // _CRS: Current Resource Settings
		{
			I2cSerialBusV2 (0x0050, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
				0x00, ResourceConsumer, , Exclusive,
				)
		})
		Name (_DEP, Package (0x01)  // _DEP: Dependencies
		{
			CAM1
		})
		Name (_PR0, Package (0x01)  // _PR0: Power Resources for D0
		{
			FCPR
		})
		Name (_PR3, Package (0x01)  // _PR3: Power Resources for D3hot
		{
			FCPR
		})
		Name (_DSD, Package (0x02)  // _DSD: Device-Specific Data
		{
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301") /* Device Properties for _DSD */,
			Package (0x06)
			{
				Package (0x02)
				{
					"size",
					0x2000
				},
				Package (0x02)
				{
					"pagesize",
					1
				},
				Package (0x02)
				{
					"read-only",
					1
				},
				Package (0x02)
				{
					"address-width",
					0x10
				},
				Package (0x02)
				{
					"compatible",
					"atmel,24c64"
				},
				Package (0x02)
				{
					"i2c-allow-low-power-probe",
					0x01
				}
			}
		})
	}
}
