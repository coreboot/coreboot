/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/pll.h>
#include <soc/pmif.h>
#include <soc/pmif_clk_common.h>
#include <soc/pmif_spmi.h>
#include <soc/pmif_sw.h>
#include <soc/spm.h>

#define OUTSEL_AD_OSC_CK_VAL	0x8

/* VLP_TOPCKGEN, VLP_CLK_CFG_UPDATE */
DEFINE_BIT(PWRAP_ULPOSC_CK_UPDATE, 4)
DEFINE_BIT(SPMI_M_TIA_32K_CK_UPDATE, 5)
DEFINE_BIT(SPMI_M_MST_CK_UPDATE, 9)

/* VLP_TOPCKGEN, VLP_CLK_CFG_0 */
DEFINE_BITFIELD(CLK_SCP_SEL, 3, 0)

/* VLP_TOPCKGEN, VLP_CLK_CFG_1 */
DEFINE_BITFIELD(CLK_PWRAP_ULPOSC_SEL, 1, 0)
DEFINE_BIT(CLK_PWRAP_ULPOSC_INV, 4)
DEFINE_BIT(PDN_PWRAP_ULPOSC, 7)
DEFINE_BITFIELD(CLK_SPMI_M_TIA_32K_SEL, 10, 8)
DEFINE_BIT(CLK_SPMI_M_TIA_32K_INV, 12)
DEFINE_BIT(PDN_SPMI_M_TIA_32K, 15)

/* VLP_TOPCKGEN, VLP_CLK_CFG_2 */
DEFINE_BITFIELD(CLK_SPMI_M_MST_SEL, 9, 8)
DEFINE_BIT(CLK_SPMI_M_MST_INV, 12)
DEFINE_BIT(PDN_SPMI_M_MST, 15)

/* VLP_TOPCKGEN, ULPOSC1_CON0 */
DEFINE_BITFIELD(ULPOSC1_CON0, 31, 0)
DEFINE_BITFIELD(OSC1_CALI_LOW, 6, 0)
DEFINE_BITFIELD(OSC1_CALI_HIGHT, 8, 7)
DEFINE_BITFIELD(OSC1_CALI, 8, 0)
DEFINE_BITFIELD(OSC1_IBAND, 15, 9)
DEFINE_BITFIELD(OSC1_FBAND, 20, 16)
DEFINE_BITFIELD(OSC1_DIV, 26, 21)
DEFINE_BIT(OSC1_CP_EN, 27)

/* VLP_TOPCKGEN, ULPOSC1_CON1 */
DEFINE_BITFIELD(ULPOSC1_CON1, 31, 0)
DEFINE_BITFIELD(OSC1_32KCALI, 7, 0)
DEFINE_BITFIELD(OSC1_LJ, 15, 8)
DEFINE_BITFIELD(OSC1_RSV, 23, 16)
DEFINE_BITFIELD(OSC1_MOD, 25, 24)
DEFINE_BITFIELD(OSC_OUTSEL, 30, 27)
DEFINE_BIT(OSC1_DIV2_EN, 26)

/* VLP_TOPCKGEN, ULPOSC1_CON2 */
DEFINE_BITFIELD(ULPOSC1_CON2, 31, 0)
DEFINE_BITFIELD(OSC1_BIAS, 7, 0)
DEFINE_BITFIELD(OSC_BUF, 15, 8)

/* TOPRGU, WDT_VLP_SWSYSRST0 */
DEFINE_BITFIELD(PMIF_RST_B, 11, 10)
DEFINE_BITFIELD(PMIFSPMI_RST_B, 13, 12)
DEFINE_BITFIELD(UNLOCK_KEY, 31, 24)
DEFINE_BIT(TIA_GRST_B, 0)

/* SPM, POWERON_CONFIG_EN */
DEFINE_BIT(BCLK_CG_EN, 0)
DEFINE_BITFIELD(PROJECT_CODE, 31, 16)

/* SPM, ULPOSC_CON */
DEFINE_BIT(ULPOSC_EN, 0)
DEFINE_BIT(ULPOSC_CG_EN, 2)

static void pmif_ulposc_config(void)
{
	/* ULPOSC1_CON0 */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_ulposc1_con[0], OSC1_CP_EN, 0, OSC1_DIV, 0x34,
			OSC1_FBAND, 0xA, OSC1_IBAND, 0x53, OSC1_CALI, 0x140);

	/* ULPOSC1_CON1 */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_ulposc1_con[1], OSC_OUTSEL, 0x2, OSC1_DIV2_EN, 0,
			OSC1_MOD, 0, OSC1_RSV, 0, OSC1_LJ, 0xA, OSC1_32KCALI, 0);

	/* ULPOSC1_CON2 */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_ulposc1_con[2], OSC1_BIAS, 0x4A, OSC_BUF, 0x48);

	printk(BIOS_INFO, "%s: ULPOSC1_CON0: 0x%x, ULPOSC1_CON1: 0x%x, ULPOSC1_CON2: 0x%x\n",
	       __func__,
	       READ32_BITFIELD(&mtk_vlpsys->vlp_ulposc1_con[0], ULPOSC1_CON0),
	       READ32_BITFIELD(&mtk_vlpsys->vlp_ulposc1_con[1], ULPOSC1_CON1),
	       READ32_BITFIELD(&mtk_vlpsys->vlp_ulposc1_con[2], ULPOSC1_CON2));
}

static void pmif_pre_ulposc_cali_bubble(u32 target_clk)
{
	unsigned int cali_val = 0;
	unsigned int abs_distance;
	unsigned int min_abs_distance = 0xFFFFFFFF;
	int min_cali_vali = 0x2;
	int64_t freq = 0;

	/* Calculate absolute delta to 260Mhz of 4 different calibration values */
	/* Try RG_OSC1_CALI[8:7] = 0x0 ~ 0x3 */
	for (cali_val = 0; cali_val < 4; cali_val++) {
		SET32_BITFIELDS(&mtk_vlpsys->vlp_ulposc1_con[0], OSC1_CALI_HIGHT, cali_val);
		freq = (int64_t)(mt_get_vlpck_freq(FREQ_METER_VLP_AD_OSC_SYNC_CK));
		abs_distance = ABS(freq - (target_clk * 1000));

		/* Memorize the minimum absolute delta and its calibration value */
		if (abs_distance < min_abs_distance) {
			min_abs_distance = abs_distance;
			min_cali_vali = cali_val;
		}
	}

	printk(BIOS_INFO, "ULPOSC1 pre-cali for high bits, select cali_val = %#x\n",
	       min_cali_vali);
	SET32_BITFIELDS(&mtk_vlpsys->vlp_ulposc1_con[0], OSC1_CALI_HIGHT, min_cali_vali);
}

static int pmif_set_osc_outsel(u32 outsel)
{
	SET32_BITFIELDS(&mtk_vlpsys->vlp_ulposc1_con[1], OSC_OUTSEL, outsel);

	if ((READ32_BITFIELD(&mtk_vlpsys->vlp_ulposc1_con[1], OSC_OUTSEL)) != outsel)
		return -1;
	else
		return 0;
}

u32 pmif_get_ulposc_freq_mhz(u32 cali_val)
{
	u32 result = 0;

	/* set calibration value */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_ulposc1_con[0], OSC1_CALI_LOW, cali_val);
	udelay(50);
	result = mt_get_vlpck_freq(FREQ_METER_VLP_AD_OSC_SYNC_CK);

	return result / 1000;
}

static int pmif_check_vlpck_freq(u32 target_clk)
{
	u32 freq_mhz;
	freq_mhz = mt_get_vlpck_freq(FREQ_METER_VLP_AD_OSC_CK) / 1000;

	/* check if calibrated value is in the range of target value +- 2% */
	if (freq_mhz > (target_clk * (1000 - CAL_MIS_RATE) / 1000) &&
	    freq_mhz < (target_clk * (1000 + CAL_MIS_RATE) / 1000))
		return 0;

	printk(BIOS_ERR, "ULPOSC1 K fail: %dM, PLL_ULPOSC1_CON0/1/2 0x%x 0x%x 0x%x\n",
	       freq_mhz,
	       READ32_BITFIELD(&mtk_vlpsys->vlp_ulposc1_con[0], ULPOSC1_CON0),
	       READ32_BITFIELD(&mtk_vlpsys->vlp_ulposc1_con[1], ULPOSC1_CON1),
	       READ32_BITFIELD(&mtk_vlpsys->vlp_ulposc1_con[2], ULPOSC1_CON2));
	return -1;
}

static void pmif_turn_onoff_ulposc(bool enable)
{
	if (enable) {
		/* turn on ulposc */
		SET32_BITFIELDS(&mtk_spm->ulposc_con, ULPOSC_EN, 1);
		udelay(100);
		SET32_BITFIELDS(&mtk_spm->ulposc_con, ULPOSC_CG_EN, 1);
	} else {
		SET32_BITFIELDS(&mtk_spm->ulposc_con, ULPOSC_EN, 0, ULPOSC_CG_EN, 0);
	}
}

static int pmif_init_ulposc(void)
{
	int ret = 0;
	/* calibrate ULPOSC1 */
	pmif_ulposc_config();
	/* enable spm swinf */
	SET32_BITFIELDS(&mtk_spm->poweron_config_en, BCLK_CG_EN, 1,
			PROJECT_CODE, 0xb16);
	pmif_turn_onoff_ulposc(true);
	pmif_pre_ulposc_cali_bubble(PMIF_ULPOSC_TARGET_FREQ_MHZ);
	ret = pmif_ulposc_cali(PMIF_ULPOSC_TARGET_FREQ_MHZ);
	pmif_turn_onoff_ulposc(false);
	ret |= pmif_set_osc_outsel(OUTSEL_AD_OSC_CK_VAL);
	pmif_turn_onoff_ulposc(true);
	ret |= pmif_check_vlpck_freq(PMIF_VLPCK_TARGET_FREQ_MHZ);

	return ret;
}

int pmif_clk_init(void)
{
	if (pmif_init_ulposc())
		return E_NODEV;

	/* for TIA
	 * [15]: pdn_spmi_m_tia [12]: clk_spmi_m_tia_inv
	 * [10:8]: clk_spmi_m_tia_sel:
	 * 0x0: tck_26m_mx9_ck, 0x1: clkrtc, 0x2: osc_d20,
	 * 0x3: osc_d14, 0x4: osc_d10
	 */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_clk_cfg[1].cfg, PDN_SPMI_M_TIA_32K, 0,
			CLK_SPMI_M_TIA_32K_INV, 0, CLK_SPMI_M_TIA_32K_SEL, 0x2);
	/* for pmif_m/p
	 * [7]: pdn_pwrap_ulposc [4]: clk_pwrap_ulposc_inv
	 * [1:0]: clk_pwrap_ulposc_sel:
	 * 0x0: tck_26m_mx9_ck, 0x1: osc_d20, 0x2: osc_d14, 0x3: osc_d10,
	 */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_clk_cfg[1].cfg, PDN_PWRAP_ULPOSC, 0,
			CLK_PWRAP_ULPOSC_INV, 0, CLK_PWRAP_ULPOSC_SEL, 0x1);
	/* for spmi_mst_m/p
	 * [15]: pdn_spmi_m_mst [12]: clk_spmi_m_mst_inv
	 * [9:8]: clk_spmi_m_mst_sel:
	 * 0x0: tck_26m_mx9_ck, 0x1: osc_d20, 0x2: osc_d14, 0x3: osc_d10,
	 */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_clk_cfg[2].cfg, PDN_SPMI_M_MST, 0,
			CLK_SPMI_M_MST_INV, 0, CLK_SPMI_M_MST_SEL, 0x1);

	/* [9]: spmi_m_mst_ck_update ,[5]: spmi_m_tia_32k_ck_update */
	/* [4]: pwrap_ulposc_ck_update */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_clk_cfg_update[0], SPMI_M_MST_CK_UPDATE, 0x1,
			SPMI_M_TIA_32K_CK_UPDATE, 0x1, PWRAP_ULPOSC_CK_UPDATE, 0x1);

	/* pmif use ULPOSC1 clock */
	/* [3]: pmicw_spmi_p_tmr_ck_26M_sel
	 * [2]: pmicw_spmi_m_tmr_ck_26M_sel
	 * [1]: spmi_mst_m_p_ulposc_32k_sel
	 * [0]: pmicw_spmi_m_ck_26M_sel
	 */
	/* Enable spmi support for ULPOSC off feature, bit[1] always set to 1 */
	SET32_BITFIELDS(&mtk_vlpsys->vlp_clk_cfg[0].set, CLK_SCP_SEL, 0x2);
	/* No extra tia clk src mux, default use ULPOSC1 clock
	 * need to confirm with DE every platform
	 */

	/* toggle SPMI-M & SPMI-P sw reset */
	/* [10]: pmifspi_m_rst_b [11]: pmifspi_p_rst_b*/
	/* [12]: pmifspi_m_rst_b [13]: pmifspi_p_rst_b*/
	SET32_BITFIELDS(&mtk_rug->wdt_vlp_swsysrst0, UNLOCK_KEY, 0x88, PMIFSPMI_RST_B, 0x3,
			PMIF_RST_B, 0x3);
	SET32_BITFIELDS(&mtk_rug->wdt_vlp_swsysrst0, UNLOCK_KEY, 0x88);

	/* [0]: tia_grst_b */
	SET32_BITFIELDS(&mtk_rug->wdt_vlp_swsysrst1, UNLOCK_KEY, 0x88, TIA_GRST_B, 0x1);
	SET32_BITFIELDS(&mtk_rug->wdt_vlp_swsysrst1, UNLOCK_KEY, 0x88);

	printk(BIOS_INFO, "%s: done\n", __func__);

	return 0;
}
