/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/smsc/lpc47n217/lpc47n217.h>
#include <ec/hp/kbc1126/ec.h>

#define SERIAL_DEV PNP_DEV(0x4e, LPC47N217_SP1)

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 }, /* Dock USB3.0 */
	{ 1, 1, 0 }, /* Conn */
	{ 1, 1, 1 }, /* USB 3.0 */
	{ 1, 1, 1 }, /* USB 3.0 */
	{ 1, 0, 2 }, /* Express Card */
	{ 1, 0, 2 }, /* Bluetooth */
	{ 0, 0, 3 },
	{ 1, 0, 3 }, /* Smart Card */
	{ 1, 1, 4 }, /* Fingerprint Reader */
	{ 1, 1, 4 }, /* Conn (Charger) */
	{ 1, 0, 5 }, /* Camera */
	{ 1, 0, 5 }, /* Dock */
	{ 1, 0, 6 }, /* WWAN */
	{ 1, 0, 6 }, /* Conn (eSATA Combo) */
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
	read_spd(&spd[1], 0x51, id_only);
	read_spd(&spd[2], 0x52, id_only);
	read_spd(&spd[3], 0x53, id_only);
}
