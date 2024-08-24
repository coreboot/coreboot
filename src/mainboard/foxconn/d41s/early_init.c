/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <northbridge/intel/pineview/pineview.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8721f/it8721f.h>

#define SERIAL_DEV PNP_DEV(0x2e, IT8721F_SP1)

void bootblock_mainboard_early_init(void)
{
	ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void get_mb_spd_addrmap(u8 *spd_addrmap)
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[1] = 0x51;
}
