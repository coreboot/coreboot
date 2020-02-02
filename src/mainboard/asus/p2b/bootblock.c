/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83977tf/w83977tf.h>

#define SERIAL_DEV PNP_DEV(0x3f0, W83977TF_SP1)

void bootblock_mainboard_early_init(void)
{
	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}
