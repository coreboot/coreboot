/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/smbus.h>
#include <intelblocks/tco.h>
#include <soc/romstage.h>

void pch_init(void)
{
	/* Programming TCO_BASE_ADDRESS and TCO Timer Halt */
	tco_configure();

	/* Program SMBUS_BASE_ADDRESS and Enable it */
	smbus_common_init();
}
