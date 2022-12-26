/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C3)
{
	PowerResource (FCPR, 0, 0)
	{
		Name (STA, 0)
		Method (_ON, 0, Serialized) {
			If (STA == 0) {
				CTXS (GPIO_FCAM_RST_L)
				STXS (GPIO_FCAM_PWR_EN)
				STXS (GPIO_PCH_FCAM_CLK_EN)
				Sleep (3)
				STXS (GPIO_FCAM_RST_L)

				/*
				 * A delay of T7 (minimum of 5 ms) + T8
				 * (max 5 ms + delay of coarse integration
				 * time value + 14 H, time for 14 horizontal
				 * lines) is needed to have the sensor ready
				 * for streaming, as soon as the power on
				 * sequence completes
				 */
				Sleep (11)
				STA = 1
			}
		}
		Method (_OFF, 0, Serialized) {
			If (STA == 1) {
				CTXS (GPIO_PCH_FCAM_CLK_EN)
				CTXS (GPIO_FCAM_RST_L)
				CTXS (GPIO_FCAM_PWR_EN)
				STA = 0
			}
		}
		Method (_STA, 0, NotSerialized) {
			Return (STA)
		}
	}

	Device (CAM0)
	{
		Name (_HID, "SONY319A") /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "Sony IMX319 Camera")  /* _DDN: DOS Device Name */

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0010, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C3",
				0x00, ResourceConsumer, ,
				)
		})

		Name (_PR0, Package () { FCPR })
		Name (_PR3, Package () { FCPR })

		/* Port0 of CAM0 is connected to port0 of CIO2 device */
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
					Package () { 1, 2, 3, 4 }
				},
				Package () { "link-frequencies",
					Package() { 482400000 }
				},
				Package () { "remote-endpoint",
					Package() { \_SB.PCI0.CIO2, 0, 0 }
				},
			}
		})
	}

	Device (NVM0)
        {
		Name (_HID, "INT3499") /* _HID: Hardware ID */
		Name (_UID, 0)  /* _UID: Unique ID */
		Name (_DDN, "M24C64S")  /* _DDN: DOS Device Name */

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0050, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C3",
				0x00, ResourceConsumer, ,)
		})
		Name (_DEP, Package () { CAM0 })

		Name (_PR0, Package () { FCPR })
		Name (_PR3, Package () { FCPR })

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
