/* SPDX-License-Identifier: GPL-2.0-only */

Device (GFX0)
{
	Name (_ADR, 0x00020000)

	/*
	 * Power state methods required for proper S3 resume support.
	 * Windows needs these to properly manage GPU power states during
	 * sleep/resume cycles.
	 */

	/* Power state D0 (fully on) - called when device is powered on */
	Method (_PS0, 0, NotSerialized)
	{
		/* Integrated graphics power is managed by the platform */
	}

	/* Power state D3 (off) - called when device is powered off */
	Method (_PS3, 0, NotSerialized)
	{
		/* Integrated graphics power is managed by the platform */
	}

	/* Deepest sleep state from S0 - D3hot */
	Method (_S0W, 0x0, NotSerialized)
	{
		Return (0x03)  /* D3hot */
	}

	/* Deepest sleep state from S3 - D3hot */
	Method (_S3D, 0x0, NotSerialized)
	{
		Return (0x03)  /* D3hot - device can enter D3hot during S3 */
	}
}
