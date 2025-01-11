/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <ec/hp/kbc1126/ec.h>

void bootblock_mainboard_early_init(void)
{
	kbc1126_enter_conf();
	kbc1126_mailbox_init();
	kbc1126_kbc_init();
	kbc1126_ec_init();
	kbc1126_pm1_init();
	kbc1126_exit_conf();
}

void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->addresses[0] = 0x50;
	/* C1S0 is a soldered RAM with no real SPD. Use stored SPD.  */
	spdi->addresses[2] = SPD_MEMORY_DOWN;
	spdi->spd_index = 0;
}
