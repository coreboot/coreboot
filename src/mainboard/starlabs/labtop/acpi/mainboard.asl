/* SPDX-License-Identifier: GPL-2.0-only */

Scope (\_SB) {
	#include "sleep.asl"

	/* Power button device. */
	Device (PWRB)
	{
		Name (_HID, EisaId ("PNP0C0C"))
		Name (PBST, One)

		Method (_STA, 0, NotSerialized)
		{
			Return (0x0F)
		}
	}
}

/*
 * The Intel Comet Lake platform doesn't support SoundWire but there
 * is a kernel bug in some 5.10.x releases.
 *
 * Debian testing live CD (at 4th Feb 2021) uses 5.10.9-1.  More
 * details can be found at https://bit.ly/3ttdffG but it appears to
 * be triggered by missing SoundWire ACPI entries.
 *
 * Add the minimal set to make it work again.
 */
Scope (_SB.PCI0.HDAS)
{
	Device (SNDW)
	{
		Name (_ADR, 0x40000000)

		Name (_CID, Package (0x02)
		{
			"PRP00001",
			"PNP0A05"
		})

		Method (_STA, 0, NotSerialized)
		{
			Return (0x0B)
		}
	}
}

/*
 * ITE IT8987E Embedded Controller
 *
 * We include this here as we need to support different levels within
 * the ACPI DSDT tree structure.
 */

#define EC_GPE_SWI 0x49 /* GPP_E15 */
#define EC_GPE_SCI 0x50 /* GPP_E16 */

#include <ec/starlabs/it8987/acpi/ec.asl>
