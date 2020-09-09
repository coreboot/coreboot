/* SPDX-License-Identifier: GPL-2.0-only */

#include <intelblocks/smbus.h>
#include <soc/romstage.h>

void romstage_pch_init(void)
{
	/* Program SMBus base address and enable it */
	smbus_common_init();
}
