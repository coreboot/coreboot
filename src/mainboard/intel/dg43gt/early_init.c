/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootblock_common.h>
#include <southbridge/intel/i82801jx/i82801jx.h>
#include <northbridge/intel/x4x/x4x.h>
#include <superio/winbond/w83627dhg/w83627dhg.h>
#include <superio/winbond/common/winbond.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627DHG_SP1)

void bootblock_mainboard_early_init(void)
{
	RCBA32(0x3410) = 0x00060464;
	RCBA32(RCBA_BUC) &= ~BUC_LAND;
	RCBA32(0x3418) = 0x01320001;
	RCBA32(0x341c) = 0xbf7f001f;
	RCBA32(0x3430) = 0x00000002;
	RCBA32(0x3f00) = 0x0000000b;

	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mb_get_spd_map(u8 spd_map[4])
{
	spd_map[0] = 0x50;
	spd_map[1] = 0x51;
	spd_map[2] = 0x52;
	spd_map[3] = 0x53;
}
