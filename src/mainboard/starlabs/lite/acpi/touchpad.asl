/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C7)
{
	Device (H02C)
	{
		Name (_HID, "STAR0001")						// _HID: Hardware ID
		Name (_CID, "PNP0C50" /* HID Protocol Device (I2C bus) */)	// _CID: Compatible ID
		Name (_UID, 0x01)						// _UID: Unique ID
		Name (_DDN, "Touchpad")						// _DDN: DOS Device Name
		Name (_DEP, Package (0x02)					// _DEP: Dependencies
		{
			GPO3,
			I2C7
		})

		Method (_STA, 0, NotSerialized)					// _STA: Status
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBusV2 (0x002C, ControllerInitiated, 0x000186A0,
				AddressingMode7Bit, "\\_SB.PCI0.I2C7",
				0x00, ResourceConsumer, , Exclusive,)
			GpioInt (Level, ActiveLow, ExclusiveAndWake, PullDefault, 0x0000,
				"\\_SB.GPO3", 0x00, ResourceConsumer, ,)
				{
					CONFIG_TRACKPAD_INTERRUPT
				}
		})

		Name (_DSD, Package (0x02)					// _DSD: Device-Specific Data
		{
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301")		/* Device Properties for _DSD */,
			Package (0x01)
			{
				Package (0x02)
				{
					"linux,probed",
					1
				}
			}
		})
		Method (_DSM, 4, Serialized)  // _DSM: Device-Specific Method
		{
			ToBuffer (Arg0, Local0)
			If ((Local0 == ToUUID ("3cdff6f7-4267-4555-ad05-b30a3d8938de")	/* HID I2C Device */))
			{
				ToInteger (Arg2, Local1)
				If (Local1 == 0x00)
				{
					ToInteger (Arg1, Local2)
					If (Local2 == 0x01)
					{
						Return (Buffer (0x01)
						{
							0x03
						})
					} Else {
						Return (Buffer (0x01)
						{
							0x00
						})
					}
				}
				If ((Local1 == 0x01))
				{
					Return (0x20)
				}
				Return (Buffer (0x01)
				{
					0x00
				})
			}
			Return (Buffer (0x01)
			{
				0x00
			})
		}
	}
}
