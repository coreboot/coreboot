/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/pll.h>
#include <soc/pll_common.h>
#include <soc/pmif.h>
#include <soc/pmif_clk_common.h>
#include <soc/pmif_spmi.h>
#include <soc/pmif_sw.h>
#include <soc/spm.h>
#include <soc/spm_mtcmos.h>

DEFINE_BITFIELD(CLK_EN, 18, 0)

/* APMIXED, ULPOSC1_CON0 */
DEFINE_BITFIELD(OSC1_CALI, 6, 0)
DEFINE_BITFIELD(OSC1_IBAND, 13, 7)
DEFINE_BITFIELD(OSC1_FBAND, 17, 14)
DEFINE_BITFIELD(OSC1_DIV, 23, 18)
DEFINE_BIT(OSC1_CP_EN, 24)

/* APMIXED, ULPOSC1_CON1 */
DEFINE_BITFIELD(OSC1_32KCALI, 7, 0)
DEFINE_BITFIELD(OSC1_RSV1, 15, 8)
DEFINE_BITFIELD(OSC1_RSV2, 23, 16)
DEFINE_BITFIELD(OSC1_MOD, 25, 24)
DEFINE_BIT(OSC1_DIV2_EN, 26)

/* APMIXED, ULPOSC1_CON2 */
DEFINE_BITFIELD(OSC1_BIAS, 7, 0)

/* SPM, POWERON_CONFIG_EN */
DEFINE_BIT(BCLK_CG_EN, 0)
DEFINE_BITFIELD(PROJECT_CODE, 31, 16)

/* SPM, ULPOSC_CON */
DEFINE_BIT(ULPOSC_EN, 0)
DEFINE_BITFIELD(ULPOSC_RST, 1, 0)

/* INFRA, PMICW_CLOCK_CTRL */
DEFINE_BITFIELD(PMIC_SYSCK_26M_SEL, 3, 0)

DEFINE_BITFIELD(CLK_PWRAP_ULPOSC_SEL, 10, 8)
DEFINE_BITFIELD(CLK_SPMI_P_MST_SEL, 19, 16)
DEFINE_BIT(CLK_PWRAP_ULPOSC_INV, 12)
DEFINE_BIT(CLK_SPMI_P_MST_INV, 20)
DEFINE_BIT(PDN_PWRAP_ULPOSC, 15)
DEFINE_BIT(PDN_SPMI_P_MST_ULPOSC, 23)

/* TOPCKGEN, CLK_CFG_UPDATE1 */
DEFINE_BITFIELD(CLK_CFG_UPDATE, 2, 1)

DEFINE_BITFIELD(SPMI_MST_RST, 13, 10)
DEFINE_BITFIELD(UNLOCK_KEY, 31, 24)

static void pmif_ulposc_config(void)
{
	SET32_BITFIELDS(&mtk_vlpsys->vlp_clk_cfg_30_set, CLK_EN, 0x70000);
	/* ULPOSC1_CON0 */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_ulposc1_con0, OSC1_CP_EN, 1, OSC1_DIV, 0xE,
			OSC1_FBAND, 0, OSC1_IBAND, 0, OSC1_CALI, 0x3D);
	/* ULPOSC1_CON1 */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_ulposc1_con1, OSC1_DIV2_EN, 1, OSC1_MOD, 0,
			OSC1_RSV2, 0, OSC1_RSV1, 0, OSC1_32KCALI, 0);
	/* ULPOSC1_CON2 */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_ulposc1_con2, OSC1_BIAS, 0x0);

	udelay(15);
}

u32 pmif_get_ulposc_freq_mhz(u32 cali_val)
{
	u32 result = 0;

	/* set calibration value */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_ulposc1_con0, OSC1_CALI, cali_val);
	udelay(50);
	result = mt_get_vlpck_freq(FREQ_METER_VLP_OSC_CK);

	return result / 1000;
}

static void pmif_turn_on_ulposc(void)
{
	/* enable APB clock swinf */
	if (!READ32_BITFIELD(&mtk_spm->poweron_config_set, BCLK_CG_EN))
		SET32_BITFIELDS(&mtk_spm->poweron_config_set, BCLK_CG_EN, 1,
				PROJECT_CODE, 0xb16);

	SET32_BITFIELDS(&mtk_spm->ulposc_con, ULPOSC_EN, 1);
	udelay(20);
	SET32_BITFIELDS(&mtk_spm->ulposc_con, ULPOSC_RST, 0x3);
	udelay(20);
	SET32_BITFIELDS(&mtk_spm->ulposc_con, ULPOSC_EN, 1);
	udelay(120);
}

static int pmif_init_ulposc(void)
{
	/* calibrate ULPOSC1 */
	pmif_ulposc_config();
	pmif_turn_on_ulposc();

	return pmif_ulposc_cali(PMIF_TARGET_FREQ_MHZ);
}

int pmif_clk_init(void)
{
	/* initialize pmif clock */
	printk(BIOS_INFO, "Using SW calibration!\n");
	if (pmif_init_ulposc())
		return E_NODEV;

	/* turn off pmic_cg_tmr, cg_ap, cg_md, cg_conn clock */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_clk_cfg[0].clr, CLK_PWRAP_ULPOSC_SEL, 0x7,
			CLK_PWRAP_ULPOSC_INV, 0x1, PDN_PWRAP_ULPOSC, 0x1);
	SET32_BITFIELDS(&mtk_vlpsys->vlp_clk_cfg[0].set, CLK_PWRAP_ULPOSC_SEL, 1);

	SET32_BITFIELDS(&mtk_vlpsys->vlp_clk_cfg[0].clr, CLK_SPMI_P_MST_SEL, 0xf,
			CLK_SPMI_P_MST_INV, 0x1, PDN_SPMI_P_MST_ULPOSC, 0x1);
	SET32_BITFIELDS(&mtk_vlpsys->vlp_clk_cfg[0].set, CLK_SPMI_P_MST_SEL, 0x3);

	SET32_BITFIELDS(&mtk_vlpsys->vlp_clk_cfg_update, CLK_CFG_UPDATE, 0x3);
	/* use ULPOSC1 clock */
	SET32_BITFIELDS(&mtk_vlpcfg->pmicw_clock_ctrl_clr, PMIC_SYSCK_26M_SEL, 0xf);

	/* toggle pwrap & SPMI-P sw reset */
	/* [10]: pmifspmi_m_rst_b [11]: pmifspmi_p_rst_b*/
	/* [12]: spmi_p_mst_rst_b [13]: pmicspi_grst_b*/
	SET32_BITFIELDS(&mtk_rug->wdt_vlp_swsysrst0, UNLOCK_KEY, 0x88, SPMI_MST_RST, 0xf);
	SET32_BITFIELDS(&mtk_rug->wdt_vlp_swsysrst0, UNLOCK_KEY, 0x88, SPMI_MST_RST, 0x0);

	return 0;
}
