/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <ec/hp/kbc1126/ec.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 },
	{ 1, 1, 0 },
	{ 1, 1, 1 },
	{ 1, 1, 1 },
	{ 1, 0, 2 },
	{ 1, 1, 2 },
	{ 0, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 1, 4 },
	{ 1, 0, 4 },
	{ 0, 0, 5 },
	{ 1, 1, 5 },
	{ 0, 0, 6 },
	{ 1, 1, 6 },
};

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

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
