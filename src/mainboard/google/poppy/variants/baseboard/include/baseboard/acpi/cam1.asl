/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C4)
{
	Device (CAM1)
	{
		Name (_HID, "INT3479") /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "OV 5670 Camera")  /* _DDN: DOS Device Name */
		Name (CAMD, 0x02)

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_DEP, Package() { \_SB.PCI0.I2C2.PMIC })
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0036, ControllerInitiated, 0x00061A80,
			AddressingMode7Bit, "\\_SB.PCI0.I2C4",
			0x00, ResourceConsumer, ,
			)
		})

		Name (_PR0, Package () {^^I2C2.PMIC.OVCM, ^^I2C2.PMIC.OVFI})
		Name (_PR3, Package () {^^I2C2.PMIC.OVCM, ^^I2C2.PMIC.OVFI})

		/* Port0 of CAM1 is connected to port1 of CIO2 device */
		Name (_DSD, Package () {
			ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package () {
				Package () { "port0", "PRT0" },
			},
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "clock-frequency", 19200000 },
			}
		})

		Name (PRT0, Package() {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "port", 0 },
			},
			ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package () {
				Package () { "endpoint0", "EP00" },
			}
		})

		Name (EP00, Package() {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "endpoint", 0 },
				Package () { "clock-lanes", 0 },
				Package () { "data-lanes",
					Package () { 1, 2 }
				},
				Package () { "link-frequencies",
					Package() { 422400000 }
				},
				Package () { "remote-endpoint",
					Package() { \_SB.PCI0.CIO2, 1, 0 }
				},
			}
		})

		Method (SSDB, 0, Serialized)
		{
			Return (Buffer (0x5E)
			{
				/* 0000 */   0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0008 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0010 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0018 */   0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x00, 0x00,
				/* 0020 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0028 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0030 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0038 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0040 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
				/* 0048 */   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x03,
				/* 0050 */   0x09, 0x00, 0x02, 0x01, 0x00, 0x01, 0x00, 0x36,
				/* 0058 */   0x6E, 0x01, 0x00, 0x00, 0x00, 0x00
			})
		}
	}
}
