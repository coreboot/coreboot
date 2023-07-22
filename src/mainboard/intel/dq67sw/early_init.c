/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <superio/winbond/w83667hg-a/w83667hg-a.h>
#include <superio/winbond/common/winbond.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83667HG_A_SP1)

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 1, 0 },
	{ 1, 1, 0 },
	{ 1, 1, 1 },
	{ 1, 1, 1 },
	{ 1, 0, 2 },
	{ 1, 0, 2 },
	{ 1, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 1, 4 },
	{ 1, 1, 4 },
	{ 0, 0, 5 },
	{ 0, 0, 5 },
	{ 1, 0, 6 },
	{ 1, 0, 6 },
};

void bootblock_mainboard_early_init(void)
{
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
