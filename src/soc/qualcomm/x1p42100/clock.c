/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <commonlib/helpers.h>
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
		.hz = 400 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(1.5),
	},
};

static struct clock_freq_config qupv3_wrap_cfg[] = {
	{
		.hz = SRC_XO_HZ,	/* 19.2MHz */
		.src = SRC_XO_19_2MHZ,
		.div = QCOM_CLOCK_DIV(1),
	},
	{
		.hz =  32 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 8,
		.n = 75,
		.d_2 = 75,
	},
	{
		.hz =  48 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 4,
		.n = 25,
		.d_2 = 25,
	},
	{
		.hz =  64 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 16,
		.n = 75,
		.d_2 = 75,
	},
	{
		.hz =  96 * MHz,
		.src = SRC_GPLL0_EVEN_300MHZ,
		.div = QCOM_CLOCK_DIV(1),
		.m = 8,
		.n = 25,
		.d_2 = 25,
	},
	{
		.hz =  100 * MHz,
		.src = SRC_GPLL0_MAIN_600MHZ,
		.div = QCOM_CLOCK_DIV(6),
	},
};

void clock_configure_qspi(uint32_t hz)
{
	clock_configure(&gcc->qspi_core, qspi_core_cfg, hz, ARRAY_SIZE(qspi_core_cfg));
	clock_enable(&gcc->qspi_cnoc_ahb_cbcr);
	clock_enable(&gcc->qspi_core_cbcr);
}

void clock_enable_qup(int qup)
{
	struct qupv3_clock *qup_clk;
	int s = qup % QUP_WRAP1_S0, clk_en_off;
	void *clk_br_en_ptr = NULL;	// Pointer to the correct apcs_clk_br_enX
	qup_clk = qup < QUP_WRAP1_S0 ? &gcc->qup_wrap0_s[s] : qup < QUP_WRAP2_S0 ?
		&gcc->qup_wrap1_s[s] : &gcc->qup_wrap2_s[s];

	if (qup <= QUP_WRAP0_S7) {
		clk_en_off = QUPV3_WRAP0_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en4;
	} else if (qup >= QUP_WRAP1_S0 && qup <= QUP_WRAP1_S6) {
		clk_en_off = QUPV3_WRAP1_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en1;
	} else if (qup >= QUP_WRAP2_S0 && qup <= QUP_WRAP2_S6) {
		clk_en_off = QUPV3_WRAP2_CLK_ENA_S(s);
		clk_br_en_ptr = &gcc->apcs_clk_br_en2;
	} else if (qup == QUP_WRAP1_S7 || qup == QUP_WRAP2_S7) {
		clk_en_off = qup == QUP_WRAP1_S7 ? QUPV3_WRAP1_SE7_CLK_ENA : QUPV3_WRAP2_SE7_CLK_ENA;
		clk_br_en_ptr = &gcc->apcs_clk_br_en2;
	}

	/* Only call if a valid pointer was assigned */
	if (clk_br_en_ptr)
		clock_enable_vote(&qup_clk->cbcr, clk_br_en_ptr, clk_en_off);
}

void clock_configure_dfsr(int qup)
{
	clock_configure_dfsr_table(qup, qupv3_wrap_cfg,
						ARRAY_SIZE(qupv3_wrap_cfg));
}
static enum cb_err clock_configure_gpll0(void)
{
	struct alpha_pll_reg_val_config gpll0_cfg = {0};

	gpll0_cfg.reg_user_ctl = &gcc->gpll0.user_ctl;

	gpll0_cfg.user_ctl_val = (read32(gpll0_cfg.reg_user_ctl) |
				1 << PLL_POST_DIV_EVEN_SHFT_X1P42100 |
				2 << PLL_POST_DIV_ODD_SHFT_X1P42100 |
				1 << PLL_PLLOUT_EVEN_SHFT_X1P42100 |
				1 << PLL_PLLOUT_MAIN_SHFT_X1P42100 |
				1 << PLL_PLLOUT_ODD_SHFT_X1P42100);

	return clock_configure_enable_gpll(&gpll0_cfg, false, 0);
}

static void speed_up_boot_cpu(void)
{
	/* Placeholder */
}

void clock_init(void)
{
	clock_configure_gpll0();
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
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP1_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_core_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP1_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_m_ahb_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP_1_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap1_s_ahb_cbcr,
				&gcc->apcs_clk_br_en1,
				QUPV3_WRAP_1_S_AHB_CLK_ENA);

	clock_enable_vote(&gcc->qup_wrap2_core_2x_cbcr,
				&gcc->apcs_clk_br_en2,
				QUPV3_WRAP2_CORE_2X_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_core_cbcr,
				&gcc->apcs_clk_br_en2,
				QUPV3_WRAP2_CORE_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_m_ahb_cbcr,
				&gcc->apcs_clk_br_en2,
				QUPV3_WRAP_2_M_AHB_CLK_ENA);
	clock_enable_vote(&gcc->qup_wrap2_s_ahb_cbcr,
				&gcc->apcs_clk_br_en2,
				QUPV3_WRAP_2_S_AHB_CLK_ENA);
	speed_up_boot_cpu();

}
