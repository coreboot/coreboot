/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/pll.h>
#include <soc/usb.h>

void mtk_usb_prepare(void)
{
	setbits32(&mtk_topckgen->clk_cfg_11_clr, BIT(7) | BIT(15));
	setbits32(&mt8195_infracfg_ao->module_sw_cg_2_clr, BIT(1) | BIT(31));
}
