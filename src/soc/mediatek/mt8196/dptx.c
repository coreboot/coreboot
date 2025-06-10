/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/dptx.h>
#include <soc/dp_intf.h>

void dptx_set_tx_power_con(void)
{
	write32p(EDP_CLK_BASE + DISP_EDPTX_PWR_CON, 0xC2FC224D);
}

void dptx_set_26mhz_clock(void)
{
	write32p(CKSYS_GP2_BASE + CKSYS2_CLK_CFG_3_CLR, 0xFF000000);
	write32p(CKSYS_GP2_BASE + CKSYS2_CLK_CFG_UPDATE, 0x00008000);
}
