/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/romstage.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8772f/it8772f.h>
#include <mainboard/google/jecht/spd/spd.h>
#include "onboard.h"

void mainboard_pre_raminit(struct romstage_params *rp)
{
	/* Fill out PEI DATA */
	mainboard_fill_pei_data(&rp->pei_data);
	mainboard_fill_spd_data(&rp->pei_data);
}

void mainboard_post_raminit(struct romstage_params *rp)
{
	if (CONFIG(CHROMEOS))
		init_bootmode_straps();
}
