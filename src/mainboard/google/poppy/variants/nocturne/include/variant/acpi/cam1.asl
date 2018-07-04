/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

Scope (\_SB.PCI0.I2C5)
{

	Device (CAM1)
	{
		Name (_HID, "SONY355A")  /* _HID: Hardware ID */
		Name (_UID, Zero)  /* _UID: Unique ID */
		Name (_DDN, "SONY IMX355A Camera") /* _DDN: DOS Device Name */

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x001A, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C5",
				0x00, ResourceConsumer, ,
				)
		})

		Name (STA, 0)
		Method (PMON, 0, Serialized) {
			If (STA == 0) {
				CTXS(GPIO_RCAM_RST_L)
				STXS(GPIO_RCAM_PWR_EN)
				STXS(GPIO_PCH_RCAM_CLK_EN)
				Sleep(3)
				STXS(GPIO_RCAM_RST_L)

				/*
				 * A delay of T7 (minimum of 10 ms) + T8
				 * (max 1.4 ms + delay of coarse integration
				 * time value) is needed to have the sensor
				 * ready for streaming, as soon as the power
				 * on sequence completes
				 */
				Sleep(12)
			}
			STA++
		}

		Method (PMOF, 0, Serialized) {
			If (STA == 0) {
				Return
			}
			STA--
			If (STA == 0) {
				CTXS(GPIO_PCH_RCAM_CLK_EN)
				CTXS(GPIO_RCAM_RST_L)
				CTXS(GPIO_RCAM_PWR_EN)
			}
		}

		Name (_PR0, Package (0x01) { RCPR })
		Name (_PR3, Package (0x01) { RCPR })

		/* Power resource methods for Rear Camera */
		PowerResource (RCPR, 0, 0) {
			Method (_ON, 0, Serialized) {
				PMON ()
			}
			Method (_OFF, 0, Serialized) {
				PMOF ()
			}
			Method (_STA, 0, Serialized) {
				If (LGreater(STA,0)) {
					Return (0x1)
				}
				Else {
					Return (0x0)
				}
			}
		}

		/* Port0 of CAM1 is connected to port1 of CIO2 device */
		Name (_DSD, Package () {
			ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
			Package () {
				Package () { "port0", "PRT0" },
			},
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "clock-frequency", 19200000 },
				Package () { "lens-focus",
					Package () { \_SB.PCI0.I2C5.VCM1 }
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
					Package() { 360000000 }
				},
				Package () { "remote-endpoint",
					Package() { \_SB.PCI0.CIO2, 1, 0 }
				},
			}
		})
	}

	Device (VCM1)
	{
		Name (_HID, "PRP0001")  /* _HID: Hardware ID */
		Name (_UID, Zero)  /* _UID: Unique ID */
		Name (_DDN, "AKM AF DAC") /* _DDN: DOS Device Name */

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x000C, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C5",
				0x00, ResourceConsumer, ,
				)
		})

		Name (_DEP, Package() { ^^I2C5.CAM1 })

		Name (_PR0, Package (0x01) { ^^I2C5.CAM1.RCPR })
		Name (_PR3, Package (0x01) { ^^I2C5.CAM1.RCPR })

		Name (_DSD, Package () {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "compatible", "asahi-kasei,ak7375" },
			}
		})
	}

	Device (NVM1)
        {
		Name (_HID, "INT3499") /* _HID: Hardware ID */
		Name (_UID, Zero)  /* _UID: Unique ID */
		Name (_DDN, "M24C64S")  /* _DDN: DOS Device Name */

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0051, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C5",
				0x00, ResourceConsumer, ,)
		})

		Name (_DEP, Package () { ^^I2C5.CAM1 })

		Name (_PR0, Package (0x01) { ^^I2C5.CAM1.RCPR })
		Name (_PR3, Package (0x01) { ^^I2C5.CAM1.RCPR })

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
