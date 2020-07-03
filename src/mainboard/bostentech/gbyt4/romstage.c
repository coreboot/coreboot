/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/mrc_wrapper.h>
#include <soc/romstage.h>

void mainboard_fill_mrc_params(struct mrc_params *mp)
{
	mp->mainboard.dram_type = DRAM_DDR3L;
	mp->mainboard.dram_info_location = DRAM_INFO_SPD_SMBUS;
	mp->mainboard.dram_is_slotted = 1;
	mp->mainboard.spd_addrs[0] = 0x50; /* Board only has one slot */
}
