/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BAYTRAIL_ROMSTAGE_H_
#define _BAYTRAIL_ROMSTAGE_H_

#include <soc/mrc_wrapper.h>

void mainboard_fill_mrc_params(struct mrc_params *mp);

void raminit(struct mrc_params *mp, int prev_sleep_state);
void gfx_init(void);
void punit_init(void);

#endif /* _BAYTRAIL_ROMSTAGE_H_ */
