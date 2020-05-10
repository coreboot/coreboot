/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB.PCI0.I2C5)
{
	/* Realtek Audio Codec */
	Device (RTEK)   /* Audio Codec driver I2C */
	{
		Name (_HID, AUDIO_CODEC_HID)
		Name (_CID, Package() { AUDIO_CODEC_CID, "INTCCFFD" })
		Name (_DDN, AUDIO_CODEC_DDN)
		Name (_UID, 1)

		Method(_CRS, 0x0, Serialized)
		{
			Name(SBUF,ResourceTemplate ()
			{
				I2CSerialBus(
					AUDIO_CODEC_I2C_ADDR,	/* SlaveAddress: bus address */
					ControllerInitiated,	/* SlaveMode: default to ControllerInitiated */
					400000,			/* ConnectionSpeed: in Hz */
					AddressingMode7Bit,	/* Addressing Mode: default to 7 bit */
					"\\_SB.PCI0.I2C5",	/* ResourceSource: I2C bus controller name */
				)

			 /* Jack Detect (index 0) */
			 GpioInt (Edge, ActiveHigh, ExclusiveAndWake, PullDefault,,
				  "\\_SB.GPSW") { JACK_DETECT_GPIO_INDEX }
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
			 "\\_SB.GPSW") { JACK_DETECT_GPIO_INDEX }
	})
}
