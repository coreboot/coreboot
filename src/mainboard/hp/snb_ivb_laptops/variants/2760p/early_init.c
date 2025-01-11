/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <ec/hp/kbc1126/ec.h>

void bootblock_mainboard_early_init(void)
{
	kbc1126_enter_conf();
	kbc1126_mailbox_init();
	kbc1126_kbc_init();
	kbc1126_ec_init();
	kbc1126_com1_init();
	kbc1126_pm1_init();
	kbc1126_exit_conf();
	kbc1126_disable4e();
}
