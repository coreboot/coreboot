/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/gpe.h>

/* XHCI Controller 0:14.0 */

Device (XHCI)
{
	Name (_ADR, 0x00140000)

	Name (_PRW, Package () { GPE0_PME_B0, 3 })

	Name (_S3D, 3)	/* D3 supported in S3 */
	Name (_S0W, 3)	/* D3 can wake device in S0 */
	Name (_S3W, 3)	/* D3 can wake system from S3 */

	Method (_PS0, 0, Serialized)
	{

	}

	Method (_PS3, 0, Serialized)
	{

	}
#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_S)
	/* Root Hub for Alder Lake-P PCH */
	Device (RHUB)
	{
		Name (_ADR, Zero)

		/* USB2 */
		Device (HS01) { Name (_ADR, 1) }
		Device (HS02) { Name (_ADR, 2) }
		Device (HS03) { Name (_ADR, 3) }
		Device (HS04) { Name (_ADR, 4) }
		Device (HS05) { Name (_ADR, 5) }
		Device (HS06) { Name (_ADR, 6) }
		Device (HS07) { Name (_ADR, 7) }
		Device (HS08) { Name (_ADR, 8) }
		Device (HS09) { Name (_ADR, 9) }
		Device (HS10) { Name (_ADR, 10) }
		Device (HS11) { Name (_ADR, 11) }
		Device (HS12) { Name (_ADR, 12) }
		Device (HS13) { Name (_ADR, 13) }
		Device (HS14) { Name (_ADR, 14) }
		/* USB3 */
		Device (SS01) { Name (_ADR, 15) }
		Device (SS02) { Name (_ADR, 16) }
		Device (SS03) { Name (_ADR, 17) }
		Device (SS04) { Name (_ADR, 18) }
		Device (SS05) { Name (_ADR, 19) }
		Device (SS06) { Name (_ADR, 20) }
		Device (SS07) { Name (_ADR, 21) }
		Device (SS08) { Name (_ADR, 22) }
		Device (SS09) { Name (_ADR, 23) }
		Device (SS10) { Name (_ADR, 24) }
	}
#else
	/* Root Hub for Alder Lake-P PCH */
	Device (RHUB)
	{
		Name (_ADR, Zero)

		/* USB2 */
		Device (HS01) { Name (_ADR, 1) }
		Device (HS02) { Name (_ADR, 2) }
		Device (HS03) { Name (_ADR, 3) }
		Device (HS04) { Name (_ADR, 4) }
		Device (HS05) { Name (_ADR, 5) }
		Device (HS06) { Name (_ADR, 6) }
		Device (HS07) { Name (_ADR, 7) }
		Device (HS08) { Name (_ADR, 8) }
		Device (HS09) { Name (_ADR, 9) }
		Device (HS10) { Name (_ADR, 10) }
		/* USB3 */
		Device (SS01) { Name (_ADR, 13) }
		Device (SS02) { Name (_ADR, 14) }
		Device (SS03) { Name (_ADR, 15) }
		Device (SS04) { Name (_ADR, 16) }
	}
#endif
}
