/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include "chip.h"
#include "raminit.h"

void get_spd_info(struct spd_info *spdi, const struct northbridge_intel_haswell_config *cfg)
{
	if (CONFIG(HAVE_SPD_IN_CBFS)) {
		/* With memory down: from mainboard code */
		mb_get_spd_map(spdi);
	} else {
		/* Without memory down: from devicetree */
		memcpy(spdi->addresses, cfg->spd_addresses, ARRAY_SIZE(spdi->addresses));
	}
}
