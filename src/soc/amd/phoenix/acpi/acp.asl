/* SPDX-License-Identifier: GPL-2.0-only */
/* ACP Audio Configuration */
Scope (\_SB.PCI0.GP41) {
	Device (ACPD) {
		/* Device addressing for ACP (Audio Coprocessor) */
		Name (_ADR, 0x05)  /* Device 0, Function 5 */

		Name (STAT, 0x3) /* Decoding Resources, Hide from UI */
		Method (_STA, 0x0, NotSerialized)
		{
			Return (STAT)
		}

		/* Child Devices - Audio endpoints */
		Device (HDA0)   /* HDA0 - HD Audio */
		{
			Name (_ADR, 0x01)
		}

		Device (PDMC)   /* PDM Controller */
		{
			Name (_ADR, 0x02)
		}

		Device (I2SC)   /* I2S Controller */
		{
			Name (_ADR, 0x03)
		}

		Device (BTSC)   /* BT Sideband Controller */
		{
			Name (_ADR, 0x04)
		}

		Device (SDWC)   /* SoundWire Controller */
		{
			Name (_ADR, 0x05)
		}

		Device (SDWS)	/* SoundWire Streaming */
		{
			Name (_ADR, 0x06)
		}

		Device (USBS)	/* USB Sideband */
		{
			Name (_ADR, 0x07)
		}
	}
}
