/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/smsc/lpc47n217/lpc47n217.h>
#include <ec/hp/kbc1126/ec.h>

#define SERIAL_DEV PNP_DEV(0x4e, LPC47N217_SP1)

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 }, /* SSP1: dock */
	{ 1, 1, 0 }, /* SSP2: left, EHCI Debug */
	{ 0, 1, 1 }, /* SSP3 */
	{ 1, 1, 1 }, /* SSP4: right */
	{ 0, 0, 2 }, /* B0P5 */
	{ 0, 0, 2 }, /* B0P6 */
	{ 0, 0, 3 }, /* B0P7 */
	{ 1, 0, 3 }, /* B0P8: smart card reader */
	{ 1, 0, 4 }, /* B1P1: fingerprint reader */
	{ 1, 0, 4 }, /* B1P2: (EHCI Debug) wlan usb */
	{ 1, 1, 5 }, /* B1P3: Camera */
	{ 1, 0, 5 }, /* B1P4 */
	{ 1, 0, 6 }, /* B1P5: wwan USB */
	{ 0, 0, 6 }, /* B1P6 */
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
