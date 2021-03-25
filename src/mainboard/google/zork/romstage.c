/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <soc/fsp.h>

void __weak variant_updm_update(FSP_M_CONFIG *mcfg) {}

void mainboard_updm_update(FSP_M_CONFIG *mcfg)
{
	variant_updm_update(mcfg);
}
