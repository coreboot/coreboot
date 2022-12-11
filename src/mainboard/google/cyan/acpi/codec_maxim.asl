/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C2)
{
	/* Maxim Audio Codec */
	Device (MAXM)   /* Audio Codec driver I2C */
	{
		Name (_HID, AUDIO_CODEC_HID)
		Name (_CID, AUDIO_CODEC_CID)
		Name (_DDN, AUDIO_CODEC_DDN)
		Name (_UID, 1)

		/* Add DT style bindings with _DSD */
		Name (_DSD, Package () {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				/* set maxim micbias to 2.8v */
				Package () { "maxim,micbias", 3 },
			}
		})

		Method(_CRS, 0x0, Serialized)
		{
			Name(SBUF,ResourceTemplate ()
			{
				I2CSerialBus(
					AUDIO_CODEC_I2C_ADDR,	/* SlaveAddress: bus address */
					ControllerInitiated,	/* SlaveMode: default to ControllerInitiated */
					400000,			/* ConnectionSpeed: in Hz */
					AddressingMode7Bit,	/* Addressing Mode: default to 7 bit */
					"\\_SB.PCI0.I2C2",	/* ResourceSource: I2C bus controller name */
				)

				GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullDefault,,
					"\\_SB.GPSE") { BOARD_JACK_MAXIM_GPIO_INDEX }
			} )
			Return (SBUF)
		}

		Method (_STA)
		{
			If (\S2EN == 1) {
				Return (0xF)
			} Else {
				Return (0x0)
			}
		}
	}
	Device (TISW)   /* TI Switch driver I2C */
	{
		Name (_HID, TI_SWITCH_HID)
		Name (_CID, TI_SWITCH_CID)
		Name (_DDN, TI_SWITCH_DDN)
		Name (_UID, 1)

		/* Add DT style bindings with _DSD */
		Name (_DSD, Package () {
			ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
			Package () {
				/* set ti micbias to 2.8v */
				Package () { "ti,micbias", 7 },
			}
		})

		Method(_CRS, 0x0, NotSerialized)
		{
			Name(SBUF,ResourceTemplate ()
			{
				I2CSerialBus(
					TI_SWITCH_I2C_ADDR,	/* SlaveAddress: bus address */
					ControllerInitiated,	/* SlaveMode: default to ControllerInitiated */
					400000,			/* ConnectionSpeed: in Hz */
					AddressingMode7Bit,	/* Addressing Mode: default to 7 bit */
					"\\_SB.PCI0.I2C2",	/* ResourceSource: I2C bus controller name */
				)

				GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullDefault,,
					"\\_SB.GPSW") { BOARD_JACK_TI_GPIO_INDEX }

			} )
			Return (SBUF)
		}

		Method (_STA)
		{
			Return (0xF)
		}
	}
}

Scope (\_SB.PCI0.LPEA)
{
	Name (GBUF, ResourceTemplate ()
	{
		/* Jack Detect (index 0) */
		GpioInt (Edge, ActiveBoth, ExclusiveAndWake, PullDefault,,
			"\\_SB.GPSE") { BOARD_JACK_MAXIM_GPIO_INDEX }
	})
}
