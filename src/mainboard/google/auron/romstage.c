/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/romstage.h>
#include "variant.h"

__weak void variant_romstage_entry(struct romstage_params *rp)
{
}

void mainboard_pre_raminit(struct romstage_params *rp)
{
	/* Fill out PEI DATA */
	mainboard_fill_pei_data(&rp->pei_data);
	mainboard_fill_spd_data(&rp->pei_data);

}

void mainboard_post_raminit(struct romstage_params *rp)
{
	/* Do variant-specific init */
	variant_romstage_entry(rp);
}
