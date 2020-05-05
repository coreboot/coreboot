/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/infracfg.h>
#include <soc/pll.h>
#include <soc/md_ctrl.h>

#define TOPCKGEN_CLK_MODE_MD_32K     (1 << 8)
#define TOPCKGEN_CLK_MODE_MD_26M     (1 << 9)
#define INFRA_MISC2_SRCCLKENA_RELEASE (0xFF)

static void internal_md_power_down(void)
{
	/* Gating MD clock */
	setbits32(&mtk_topckgen->clk_mode,
		TOPCKGEN_CLK_MODE_MD_32K | TOPCKGEN_CLK_MODE_MD_26M);
	/* Release SRCCLKENA */
	clrbits32(&mt8183_infracfg->infra_misc2,
		INFRA_MISC2_SRCCLKENA_RELEASE);
}

void mtk_md_early_init(void)
{
	internal_md_power_down();
}
