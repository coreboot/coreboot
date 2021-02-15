/* SPDX-License-Identifier: GPL-2.0-only */

#include "../../variant.h"

void variant_memory_init_params(FSP_M_CONFIG *mem_cfg)
{
	/* Enable and set SATA HSIO adjustments for ports 0 and 2 */
	mem_cfg->PchSataHsioRxGen3EqBoostMagEnable[0] = 1;
	mem_cfg->PchSataHsioRxGen3EqBoostMagEnable[2] = 1;
	mem_cfg->PchSataHsioRxGen3EqBoostMag[0] = 2;
	mem_cfg->PchSataHsioRxGen3EqBoostMag[2] = 1;

	/* Enable Refresh2X to mitigate S3 resume issues */
	mem_cfg->Refresh2X = 1;
}
