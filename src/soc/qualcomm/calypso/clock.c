/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/clock.h>
#include <types.h>

static struct clock_freq_config qspi_core_cfg[] = {
	{
		.hz = SRC_XO_HZ,	/* 19.2MHz */
		.src = SRC_XO_19_2MHZ,
		.div = QCOM_CLOCK_DIV(1),
	},
	{
		.hz = 100 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(6),
	},
	{
		.hz = 150 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(4),
	},
	{
		.hz = 200 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(3),
	},
	{
		.hz = 300 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(2),
	},
	{
		.hz = 400 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(1.5),
	},
};

void clock_configure_qspi(uint32_t hz)
{
	clock_configure(&gcc->qspi_core, qspi_core_cfg, hz,
			ARRAY_SIZE(qspi_core_cfg));
	clock_enable(&gcc->qspi_cnoc_ahb_cbcr);
	clock_enable(&gcc->qspi_core_cbcr);
}

/* Helper function to get SE index within a wrap */
static inline int get_qup_se_index(int qup)
{
	return qup % QUP_WRAP1_S0;
}

/* Helper function to get pointer to QUP SE clock structure */
static void *get_qup_se_clk(int qup, int s)
{
	struct calypso_qupv3_wrap *wrap = NULL;
	void *se_clk_array[8];

	/* Determine which wrap based on qup value */
	if (qup <= QUP_WRAP0_S7)
		wrap = qup_wrap0_clk;
	else if (qup <= QUP_WRAP1_S7)
		wrap = qup_wrap1_clk;
	else if (qup <= QUP_WRAP2_S7)
		wrap = qup_wrap2_clk;
	else if (qup <= QUP_WRAP3_S1)
		wrap = qup_oob_clk;

	if (!wrap) {
		printk(BIOS_ERR, "%s: invalid QUP index %d\n", __func__, qup);
		return NULL;
	}

	/* Build array of pointers to SE clock structures */
	se_clk_array[0] = &wrap->qupv3_s0;
	se_clk_array[1] = &wrap->qupv3_s1;
	se_clk_array[2] = &wrap->qupv3_s2;
	se_clk_array[3] = &wrap->qupv3_s3;
	se_clk_array[4] = &wrap->qupv3_s4;
	se_clk_array[5] = &wrap->qupv3_s5;
	se_clk_array[6] = &wrap->qupv3_s6;
	se_clk_array[7] = &wrap->qupv3_s7;
	return se_clk_array[s];
}

void clock_enable_qup(int qup)
{
	int s = get_qup_se_index(qup), clk_en_off;
	void *clk_br_en_ptr = NULL;
	void *se_clk;
	u32 *cbcr;

	if (qup <= QUP_WRAP0_S7) {
		clk_en_off = QUPV3_WRAP0_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en4;
	} else if (qup <= QUP_WRAP1_S7) {
		clk_en_off = QUPV3_WRAP1_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en3;
	} else if (qup <= QUP_WRAP2_S7) {
		if (qup <= QUP_WRAP2_S1) {
			clk_en_off = (qup == QUP_WRAP2_S0) ?
				QUPV3_WRAP2_SE0_CLK_ENA :
				QUPV3_WRAP2_SE1_CLK_ENA;
			clk_br_en_ptr = &gcc->apcs_clk_br_en3;
		} else {
			clk_en_off = QUPV3_WRAP2_CLK_ENA_S(s);
			clk_br_en_ptr = &gcc->apcs_clk_br_en4;
		}
	} else if (qup <= QUP_WRAP3_S1) {
		clk_en_off = QUPV3_OOB_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en3;
	}

	se_clk = get_qup_se_clk(qup, s);
	if (!se_clk)
		return;

	if (s == 2 || s == 3)
		cbcr = &((struct qupv3_clock_v2 *)se_clk)->cbcr;
	else
		cbcr = &((struct qupv3_clock *)se_clk)->cbcr;

	/* Only call if a valid pointer was assigned */
	if (clk_br_en_ptr)
		clock_enable_vote(cbcr, clk_br_en_ptr, clk_en_off);
}

void clock_configure_dfsr(int qup)
{
	/* placeholder */
}

enum cb_err pll_init_and_set(struct calypso_ncc0_clock *ncc0, u32 l_val)
{
	int ret;
	struct alpha_pll_reg_val_config ncc0_pll_cfg = {0};

	setbits64p(NCC0_NCC_CMU_NCC_CLK_CFG,
		   BIT(PLLSWCTL) | BIT(OVRCKMUXPLLFASTCLK));

	setbits64p(NCC0_NCC_CMU_NCC_PLL_CFG,
		   ((uint64_t)(LOCKTMOUTCNT_VAL & LOCKTMOUTCNT_BMSK)
		    << LOCKTMOUTCNT) |
		   ((uint64_t)(LOCKDEASSERTTMOUTCNT_VAL & LOCKDEASSERTTMOUTCNT_BMSK)
		    << LOCKDEASSERTTMOUTCNT));

	ncc0_pll_cfg.reg_config_ctl = &ncc0->pll0_config_ctl;
	ncc0_pll_cfg.config_ctl_val = read32(ncc0_pll_cfg.reg_config_ctl) |
				      PFA_MSB_VAL << PFA_MSB |
				      RON_DEGEN_MULTIPLY_VAL << RON_DEGEN_MULTIPLY |
				      FBC_ALPHA_CAL_VAL << FBC_ALPHA_CAL |
				      PLL_COUNTER_ENABLE_VAL << PLL_COUNTER_ENABLE;

	ncc0_pll_cfg.reg_config_ctl_hi = &ncc0->pll0_config_ctl_u;
	ncc0_pll_cfg.config_ctl_hi_val = read32(ncc0_pll_cfg.reg_config_ctl_hi) |
					 CHP_REF_CUR_TRIM_VAL << CHP_REF_CUR_TRIM |
					 ADC_KLSB_VALUE_VAL << ADC_KLSB_VALUE |
					 ADC_KMSB_VALUE_VAL << ADC_KMSB_VALUE;

	ncc0_pll_cfg.reg_l = &ncc0->pll0_l;
	ncc0_pll_cfg.l_val = l_val;

	ncc0_pll_cfg.reg_alpha = &ncc0->pll0_alpha;
	ncc0_pll_cfg.alpha_val = 0x00;
	ncc0_pll_cfg.reg_user_ctl = &ncc0->pll0_user_ctl;
	ncc0_pll_cfg.user_ctl_val = (1 << PLL_PLLOUT_EVEN_SHFT |
			1 << PLL_PLLOUT_MAIN_SHFT);

	if (clock_configure_enable_gpll(&ncc0_pll_cfg, false, 0) != CB_SUCCESS)
		return CB_ERR;

	ncc0_pll_cfg.reg_mode = &ncc0->pll0_mode;
	ncc0_pll_cfg.reg_opmode = &ncc0->pll0_opmode;
	ret = zondaole_pll_enable(&ncc0_pll_cfg);
	if (ret != CB_SUCCESS)
		return CB_ERR;

	setbits64p(NCC0_NCC_CMU_NCC_CLK_CFG, BIT(SELCKMUXPLLFASTCLK));

	return CB_SUCCESS;
}

static enum cb_err clock_configure_gpll0(void)
{
	struct alpha_pll_reg_val_config gpll0_cfg = {0};

	gpll0_cfg.reg_user_ctl = &gcc->gpll0.user_ctl;
	gpll0_cfg.user_ctl_val = read32(gpll0_cfg.reg_user_ctl) |
				 BIT(PLL_PLLOUT_MAIN_SHFT_CALYPSO) |
				 BIT(PLL_PLLOUT_EVEN_SHFT_CALYPSO) |
				 BIT(PLL_PLLOUT_ODD_SHFT_CALYPSO) |
				 1 << PLL_POST_DIV_EVEN_SHFT_CALYPSO |
				 2 << PLL_POST_DIV_ODD_SHFT_CALYPSO;

	return clock_configure_enable_gpll(&gpll0_cfg, false, 0);
}

static void speed_up_boot_cpu(void)
{
	/* 1363.2 MHz */
	if (pll_init_and_set(apss_ncc0, L_VAL_1363P2MHz) == CB_SUCCESS)
		printk(BIOS_DEBUG, "NCC frequency bumped to 1363.2 MHz\n");
	else
		printk(BIOS_ERR, "NCC PLL initialization failed\n");
}

void clock_init(void)
{
	if (clock_configure_gpll0() != CB_SUCCESS)
		printk(BIOS_ERR, "GPLL0 configuration failed\n");

	clock_enable_vote(&gcc->qup_wrap0_core_2x_cbcr,
			  &gcc->apcs_clk_br_en4,
			  QUPV3_WRAP0_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_core_cbcr,
			  &gcc->apcs_clk_br_en4,
			  QUPV3_WRAP0_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_m_ahb_cbcr,
			  &gcc->apcs_clk_br_en4,
			  QUPV3_WRAP_0_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap0_s_ahb_cbcr,
			  &gcc->apcs_clk_br_en4,
			  QUPV3_WRAP_0_S_AHB_CLK_ENA);

	clock_enable_vote(&gcc->qup_wrap1_core_2x_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP1_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_core_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP1_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_m_ahb_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP_1_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_s_ahb_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP_1_S_AHB_CLK_ENA);

	clock_enable_vote(&gcc->qup_wrap2_core_2x_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP2_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_core_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP2_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_m_ahb_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP_2_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_s_ahb_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_WRAP_2_S_AHB_CLK_ENA);

	clock_enable_vote(&gcc->qup_oob_core_2x_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_OOB_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_oob_core_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_OOB_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_oob_s_ahb_cbcr,
			  &gcc->apcs_clk_br_en3,
			  QUPV3_OOB_S_AHB_CLK_ENA);

	speed_up_boot_cpu();
}
