/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C3)
{
	/* Power resource methods for Front Camera */
	PowerResource (FCPR, 0, 0) {
		Name (STA, 0)
		Method (_ON, 0, Serialized) {
			PMON ()
		}
		Method (_OFF, 0, Serialized) {
			PMOF ()
		}
		Method (_STA, 0, Serialized) {
			If (STA > 0) {
				Return (0x1)
			}
			Else {
				Return (0x0)
			}
		}
		Method (PMON, 0, Serialized) {
			If (STA == 0) {
				CTXS (EN_CAM_PMIC_RST_L)
				STXS (EN_PP3300_DX_CAM)
				Sleep(3)
				STXS (EN_CAM_PMIC_RST_L)
				STXS (EN_CAM_CLOCK)
				Sleep(2);
			}
			STA++
		}

		Method (PMOF, 0, Serialized) {
			If (STA == 0) {
				Return
			}
			STA--
			If (STA == 0) {
				CTXS (EN_CAM_CLOCK)
				CTXS (EN_CAM_PMIC_RST_L)
				CTXS (EN_PP3300_DX_CAM)
			}
		}
	}

	Device (CAM0)
	{
		Name (_HID, "INT3478") /* _HID: Hardware ID */
		Name (_UID, Zero)  /* _UID: Unique ID */
		Name (_DDN, "SONY IMX208 Camera")  /* _DDN: DOS Device Name */

		Method (_STA, 0, NotSerialized)  /* _STA: Status */
		{
			Return (0x0F)
		}

		Name (_CRS, ResourceTemplate ()
		{
			I2cSerialBus (0x0010, ControllerInitiated, 0x00061A80,
				AddressingMode7Bit, "\\_SB.PCI0.I2C3",
				0x00, ResourceConsumer, ,)
		})

		Name (_PR0, Package (0x01) { FCPR })
		Name (_PR3, Package (0x01) { FCPR })

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
					Package () { 1, 2 }
				},
				Package () { "link-frequencies",
					Package() { 384000000, 96000000 }
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
		Name (_UID, Zero)  /* _UID: Unique ID */
		Name (_DDN, "GT24C16S")  /* _DDN: DOS Device Name */

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
		Name (_PR0, Package (0x01) { FCPR })
		Name (_PR3, Package (0x01) { FCPR })

		Name (_DSD, Package ()
		{
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				Package () { "size", 2048 },
				Package () { "pagesize", 1 },
				Package () { "read-only", 1 },
				Package () { "address-width", 8 },
			}
		})
	}
}
