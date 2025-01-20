/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/dvfsrc.h>

void dvfsrc_opp_level_mapping(void)
{
	setbits32p(DVFSRC_RSRV_4, BIT(VCORE_B0_SHIFT));
	printk(BIOS_INFO, "Vcore DVFS settings done\n");
}
