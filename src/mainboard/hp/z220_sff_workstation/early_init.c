/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <superio/nuvoton/npcd378/npcd378.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>

#define SERIAL_DEV PNP_DEV(0x2e, NPCD378_SP2)

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 1, 5 },
	{ 1, 0, 5 },
	{ 1, 0, 5 },
	{ 1, 0, 5 },
	{ 1, 0, 7 },
	{ 1, 0, 7 },
};

void bootblock_mainboard_early_init(void)
{
	if (CONFIG(CONSOLE_SERIAL))
		nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	/* BTX mainboard: Reversed mapping */
	read_spd(&spd[3], 0x50, id_only);
	read_spd(&spd[2], 0x51, id_only);
	read_spd(&spd[1], 0x52, id_only);
	read_spd(&spd[0], 0x53, id_only);
}
