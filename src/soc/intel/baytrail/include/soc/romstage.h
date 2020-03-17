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

#ifndef _BAYTRAIL_ROMSTAGE_H_
#define _BAYTRAIL_ROMSTAGE_H_

#include <stdint.h>
#include <soc/mrc_wrapper.h>

void mainboard_fill_mrc_params(struct mrc_params *mp);

void raminit(struct mrc_params *mp, int prev_sleep_state);
void gfx_init(void);
void punit_init(void);

#endif /* _BAYTRAIL_ROMSTAGE_H_ */
