/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/reset.h>
#include <cf9_reset.h>
#include <soc/soc_util.h>

void cf9_reset_prepare(void)
{
	if (get_soc_type() == SOC_GLINDA)
		set_resets_to_cold();
}
