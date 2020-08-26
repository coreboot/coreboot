/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/smbus.h>
#include <soc/romstage.h>

void romstage_pch_init(void)
{
	/* Program SMBUS_BASE_ADDRESS and Enable it */
	smbus_common_init();
}
