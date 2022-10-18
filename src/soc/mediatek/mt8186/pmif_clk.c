/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/pll.h>
#include <soc/pll_common.h>
#include <soc/pmif.h>
#include <soc/pmif_clk_common.h>
#include <soc/pmif_sw.h>

#define FREQ_METER_ABIST_AD_OSC_CK	35

#define SCP_CLK_SRC			BIT(1)
#define SCP_CLK_CG			BIT(2)

#define ULPOSC1_RG_OSC_DIV		BIT(18)

#define ULPOSC1_CALI_HW_MODE		(BIT(16) | BIT(17))
#define ULPOSC1_ENABLE_SW_MODE		(BIT(18) | BIT(19))

u32 pmif_get_ulposc_freq_mhz(u32 cali_val)
{
	u32 result;

	clrsetbits32(&mtk_apmixed->ap_pll_con0, ULPOSC1_CALI_HW_MODE, ULPOSC1_ENABLE_SW_MODE);
	clrbits32(&mtk_apmixed->ulposc_con0, cali_val);

	/* enable ulposc1 */
	setbits32(&mtk_scp_clk->scp_clk_en, SCP_CLK_SRC);
	udelay(150);
	setbits32(&mtk_scp_clk->scp_clk_en, SCP_CLK_CG);

	result = mt_fmeter_get_freq_khz(FMETER_ABIST, FREQ_METER_ABIST_AD_OSC_CK);

	return result / 1000;
}

int pmif_clk_init(void)
{
	u32 ulposc;

	/* get hardware default value */
	ulposc = pmif_get_ulposc_freq_mhz(ULPOSC1_RG_OSC_DIV);
	if (pmif_ulposc_check(ulposc, FREQ_250MHZ))
		die("ERROR: failed to meet ulposc frequency\n");

	mt_pll_spmi_mux_select();

	return 0;
}
