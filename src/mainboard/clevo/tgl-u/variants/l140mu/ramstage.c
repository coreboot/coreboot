/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <variant/ramstage.h>

void variant_configure_fsps(FSP_S_CONFIG *params)
{
	/* SSD1 - PCIe4 */
	params->CpuPcieRpLtrEnable[0] = 1;
	params->CpuPcieRpSlotImplemented[0] = 1;
}
