/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Google Inc.
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


/* Grunt specific I2S machine driver */
Device (I2S)
{
	Name (_ADR, 1)
	Name (_HID, "AMD7219")
	Name (_CID, "AMD7219")
}

/* DA7219 codec */
Device (DLG7)
{
	Name (_HID, "DLGS7219")
	Name (_UID, 1)
	Name (_DDN, "Dialog Semiconductor DA7219 Audio Codec")
	Name (_S0W, 0x04)

	Name(_CRS, ResourceTemplate ()
	{
		I2cSerialBus (
			0x001A,
			ControllerInitiated,
			0x00061A80,
			AddressingMode7Bit,
			"\\_SB.I2CA",
			0x00,
			ResourceConsumer,
			,
		)

		/* Jack Detect AGPIO14 */
		GpioInt (Edge, ActiveLow, ExclusiveAndWake, PullNone,,
			"\\_SB.GPIO") { 14 }
	})

	/* Device-Specific Data */
	Name (_DSD, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package () { "dlg,micbias_lvl", 2600 },
			Package () { "dlg,mic_amp_in_sel", "diff" }
		},

		ToUUID ("dbb8e3e6-5886-4ba6-8795-1319f52a966b"),
		Package ()
		{
			Package () { "da7219_aad", "DAAD" }
		}
	})

	/* Device Properties for _DSD */
	Name (DAAD, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package () { "dlg,btn-cfg", 50 },
			Package () { "dlg,mic-det-thr", 500 },
			Package () { "dlg,jack-ins-deb", 20 },
			Package () { "dlg,jack-det-rate", "32ms_64ms" },
			Package () { "dlg,jack-rem-deb", 1 },
			Package () { "dlg,a-d-btn-thr", 0xa },
			Package () { "dlg,d-b-btn-thr", 0x16 },
			Package () { "dlg,b-c-btn-thr", 0x21 },
			Package () { "dlg,c-mic-btn-thr", 0x3e  },
			Package () { "dlg,btn-avg", 4 },
			Package () { "dlg,adc-1bit-rpt", 1 },
			Package () { "dlg,micbias-pulse-lvl", 0 },
			Package () { "dlg,micbias-pulse-time", 0 }
		}
	}) /* DAAD */

	Method (_STA, 0, NotSerialized)
	{
		Return (0x0F)
	}
}

/* MAX98357 codec */
Device (MAXM)
{
	Name (_HID, "MX98357A")
	Name (_UID, 0)
	Name (_DDN, "Maxim Integrated 98357A Amplifier")

	Name (_CRS, ResourceTemplate ()
	{
		GpioIo (Exclusive, PullDefault, 0x0000, 0x0000,
			IoRestrictionOutputOnly,
			"\\_SB.GPIO", 0x00, ResourceConsumer, ,) { 119 }
	})

	Name (_DSD, Package ()
	{
		ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
		Package ()
		{
			Package ()
			{
				"sdmode-gpio", Package () { \MAXM, 0, 0, 0 }
			},

			Package ()
			{
				"sdmode-delay", 5
			}
		}
	})

	Method (_STA, 0, NotSerialized)
	{
		Return (0x0F)
	}
}
