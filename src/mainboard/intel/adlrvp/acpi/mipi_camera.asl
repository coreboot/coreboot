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
				2
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
				One
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
	Name (EP00, Package (0x02)
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
					^I2C1.CAM0,
					Zero,
					Zero
				}
			}
		}
	})
}

Scope (\_SB.PCI0.I2C1)
{
	PowerResource (RCPR, 0x00, 0x0000)
	{
		Name (STA, Zero)
		Method (_ON, 0, Serialized)  /* Rear camera_ON_: Power On */
		{
			If ((STA == Zero))
			{
				/* Enable IMG_CLK */
				MCON(0,1) /* Clock 0, 19.2MHz */
				/* Pull PWREN high */
				STXS(GPP_B23)
				Sleep(2) /* reset pulse width */
				/* Pull RST low */
				CTXS(GPP_R5)
				Sleep(1) /* t2 */
				/* Pull RST high */
				STXS(GPP_R5)
				Sleep(1) /* t2 */
				Store(1,STA)
			}
		}
		Method (_OFF, 0, Serialized)  /* Rear camera _OFF: Power Off */
		{
			If ((STA == One))
			{
				/* Disable IMG_CLK */
				Sleep(1) /* t0+t1 */
				MCOF(0) /* Clock 0 */
				/* Pull RST low */
				CTXS(GPP_R5)
				/* Pull PWREN low */
				CTXS(GPP_B23)
				Store(0,STA)
			}
		}
		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (STA)
		}
	}
	Device (CAM0)
	{
		Name (_HID, "OVTI5675")  /* _HID: Hardware ID */
		Name (_UID, Zero)  /* _UID: Unique ID */
		Name (_DDN, "Ov 5675 Camera")  /* _DDN: DOS Device Name */
		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}
		Name (_CRS, ResourceTemplate ()  /* _CRS: Current Resource Settings */
		{
			I2cSerialBus (0x0036, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C1",
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
			Package (0x02)
			{
				Package (0x02)
				{
					"clock-frequency",
					0x0124F800
				},
				Package (0x02)
				{
					"lens-focus",
					Package (0x01)
					{
						VCM0
					}
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
					Package (0x02)
					{
						One,
						0x02
					}
				},
				Package (0x02)
				{
					"link-frequencies",
					Package (0x01)
					{
						0x1AD27480
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
	Device (VCM0)
	{
		Name (_HID, "PRP0001")  /* _HID: Hardware ID */
		Name (_UID, 0x03)  /* _UID: Unique ID */
		Name (_DDN, "DW AF DAC")  /* _DDN: DOS Device Name */
		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}
		Name (_CRS, ResourceTemplate ()  /* _CRS: Current Resource Settings */
		{
			I2cSerialBus (0x000C, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C1",
				0x00, ResourceConsumer, ,
			)
		})
		Name (_DEP, Package (0x01)  /* _DEP: Dependencies */
		{
			CAM0
		})
		Name (_PR0, Package (0x01)  /* _PR0: Power Resources for D0 */
		{
			RCPR
		})
		Name (_PR3, Package (0x01)  /* _PR3: Power Resources for D3hot */
		{
			RCPR
		})
		Name (_DSD, Package (0x02)  /* _DSD: Device-Specific Data */
		{
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package (0x01)
			{
				Package (0x02)
				{
					"compatible",
					"dongwoon,dw9714"
				}
			}
		})
	}
}

Scope (\_SB.PCI0.I2C5)
{
	PowerResource (FCPR, 0x00, 0x0000)
	{
		Name (STA, Zero)
		Method (_ON, 0, Serialized)  /* Front camera_ON_: Power On */
		{
			If ((STA == Zero))
			{
				/* Enable IMG_CLK */
				MCON(1,1) /* Clock 1, 19.2MHz */
				/* Pull PWREN high */
				STXS(GPP_E16)
				Sleep(2) /* reset pulse width */
				/* Pull RST low */
				CTXS(GPP_E15)
				Sleep(1) /* t2 */
				/* Pull RST high */
				STXS(GPP_E15)
				Sleep(1) /* t2 */
				Store(1,STA)
			}
		}
		Method (_OFF, 0, Serialized)  /* Front camera_OFF_: Power Off */
		{
			If ((STA == One))
			{
				/* Disable IMG_CLK */
				Sleep(1) /* t0+t1 */
				MCOF(1) /* Clock 1 */
				/* Pull RST low */
				CTXS(GPP_E15)
				/* Pull PWREN low */
				CTXS(GPP_E16)
				Store(0,STA)
			}
		}
		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (STA)
		}
	}
	Device (CAM1)
	{
		Name (_HID, "OVTI5675")  /* _HID: Hardware ID */
		Name (_UID, Zero)  /* _UID: Unique ID */
		Name (_DDN, "Ov 5675 Camera")  /* _DDN: DOS Device Name */
		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}
		Name (_CRS, ResourceTemplate ()  /* _CRS: Current Resource Settings */
		{
			I2cSerialBus (0x0036, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C5",
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
					Package (0x02)
					{
						One,
						0x02
					}
				},
				Package (0x02)
				{
					"link-frequencies",
					Package (0x01)
					{
						0x1AD27480
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
