/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

/*
 * This file is created based on MT8189 Functional Specification
 * Chapter number: 10.28
 */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/dptx.h>
#include <soc/dp_intf.h>

void dptx_set_tx_power_con(void)
{
}

void dptx_set_26mhz_clock(void)
{
	write32p(CKSYS_BASE + CKSYS_CLK_CFG_16_CLR, 0xFF000000);
	write32p(CKSYS_BASE + CKSYS_CLK_CFG_UPDATE, 0x00000020);
}
