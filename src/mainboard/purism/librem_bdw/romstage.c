/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/romstage.h>

void mainboard_pre_raminit(struct romstage_params *rp)
{
	/* Fill out PEI DATA */
	mainboard_fill_pei_data(&rp->pei_data);
}

void mainboard_post_raminit(struct romstage_params *rp)
{
}
