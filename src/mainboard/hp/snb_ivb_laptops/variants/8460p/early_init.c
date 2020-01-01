/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Iru Cai <mytbk920423@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootblock_common.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/smsc/lpc47n217/lpc47n217.h>
#include <ec/hp/kbc1126/ec.h>

#define SERIAL_DEV PNP_DEV(0x4e, LPC47N217_SP1)

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 }, /* USB0, eSATA */
	{ 1, 0, 0 }, /* USB charger */
	{ 0, 1, 1 },
	{ 1, 1, 1 }, /* camera */
	{ 1, 0, 2 }, /* USB4 expresscard */
	{ 1, 0, 2 }, /* bluetooth */
	{ 0, 0, 3 },
	{ 1, 0, 3 }, /* smartcard */
	{ 1, 1, 4 }, /* fingerprint */
	{ 1, 1, 4 }, /* WWAN */
	{ 1, 0, 5 }, /* CONN */
	{ 1, 0, 5 }, /* docking */
	{ 1, 0, 6 }, /* CONN */
	{ 1, 0, 6 }, /* docking */
};

void bootblock_mainboard_early_init(void)
{
	lpc47n217_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	kbc1126_enter_conf();
	kbc1126_mailbox_init();
	kbc1126_kbc_init();
	kbc1126_ec_init();
	kbc1126_pm1_init();
	kbc1126_exit_conf();
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
