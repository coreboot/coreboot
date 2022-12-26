/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C2)
{
	Device (CAM0)
	{
		Name (_HID, "SONY258A")  /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "SONY IMX258 Camera") /* _DDN: DOS Device Name */

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_DEP, Package() { \_SB.PCI0.I2C2.PMIC })
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x001A, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
				0x00, ResourceConsumer, ,
				)
		})

		Name (_PR0, Package () { ^^I2C2.PMIC.OVCM, ^^I2C2.PMIC.OVTH })
		Name (_PR3, Package () { ^^I2C2.PMIC.OVCM, ^^I2C2.PMIC.OVTH })

		/* Port0 of CAM0 is connected to port0 of CIO2 device */
		Name (_DSD, Package () {
			ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package () {
				Package () { "port0", "PRT0" },
			},
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "clock-frequency", 19200000 },
				Package () { "rotation", 180 },
				Package () { "lens-focus",
					Package () { \_SB.PCI0.I2C2.VCM0 }
				}
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
					Package () { 1, 2, 3, 4 }
				},
				Package () { "link-frequencies",
					Package() { 633600000, 320000000 }
				},
				Package () { "remote-endpoint",
					Package() { \_SB.PCI0.CIO2, 0, 0 }
				},
			}
		})
	}

	Device (VCM0)
	{
		Name (_HID, "PRP0001")  /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "Dongwoon AF DAC") /* _DDN: DOS Device Name */

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_DEP, Package() { \_SB.PCI0.I2C2.PMIC })
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x000C, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
				0x00, ResourceConsumer, ,
				)
		})

		Name (_DSD, Package () {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "compatible", "dongwoon,dw9807" },
			}
		})

		Name (_PR0, Package () { ^PMIC.VCMP })
		Name (_PR3, Package () { ^PMIC.VCMP })
	}

	Device (NVM0)
	{
		Name (_HID, "INT3499")  /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "Dongwoon NVM")  /* _DDN: DOS Device Name */

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_DEP, Package() { \_SB.PCI0.I2C2.PMIC })
		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0058, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C2",
				0x00, ResourceConsumer, ,)
		})

		Name (_PR0, Package () { ^PMIC.VCMP })
		Name (_PR3, Package () { ^PMIC.VCMP })

		Name (_DSD, Package ()
		{
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "size", 8192 },
				Package () { "pagesize", 1 },
				Package () { "read-only", 1 },
				Package () { "address-width", 16 },
			}
		})
	}
}
