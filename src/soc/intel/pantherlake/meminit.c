/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <fsp/util.h>
#include <soc/meminit.h>
#include <string.h>

void memcfg_init(FSPM_UPD *memupd, const struct mb_cfg *mb_cfg,
		 const struct mem_spd *spd_info, bool half_populated)
{
	/* Update after FSP is released externally released. */
}
