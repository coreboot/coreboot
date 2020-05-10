 /* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <types.h>
#include <commonlib/helpers.h>
#include <soc/clock.h>

#define DIV(div) (div ? (2*div - 1) : 0)
#define HALF_DIVIDER(div2x)  (div2x ? (div2x - 1) : 0)

struct clock_config uart_cfg[] = {
	{
		.hz = 1843200,
		.hw_ctl = 0x0,
		.src = SRC_GPLL0_MAIN_800MHZ,
		.div = DIV(0),
		.m = 36,
		.n = 15625,
		.d_2 = 15625,
	},
	{
		.hz = 3686400,
		.hw_ctl = 0x0,
		.src = SRC_GPLL0_MAIN_800MHZ,
		.div = DIV(0),
		.m = 72,
		.n = 15625,
		.d_2 = 15625,
	}
};

struct clock_config i2c_cfg[] = {
	{
		.hz = 19200000,
		.hw_ctl = 0x0,
		.src = SRC_XO_19_2MHZ,
		.div = DIV(0),
	},
	{
		.hz = 50000000,
		.hw_ctl = 0x0,
		.src = SRC_GPLL0_MAIN_800MHZ,
		.div = DIV(32),
	}
};

struct clock_config spi_cfg[] = {
	{
		.hz = 1000000,
		.hw_ctl = 0x0,
		.src = SRC_XO_19_2MHZ,
		.div = DIV(48),
	},
	{
		.hz = 7372800,
		.src = SRC_GPLL0_MAIN_800MHZ,
		.div = DIV(1),
		.m = 144,
		.n = 15625,
		.d_2 = 15625,
	},
	{
		.hz = 19200000,
		.hw_ctl = 0x0,
		.src = SRC_XO_19_2MHZ,
		.div = DIV(0),
	},
	{
		.hz = 30000000,
		.hw_ctl = 0x0,
		.src = SRC_XO_19_2MHZ,
		.div = DIV(0),
	},
	{
		.hz = 50000000,
		.hw_ctl = 0x0,
		.src = SRC_GPLL0_MAIN_800MHZ,
		.div = DIV(32),
	}
};

static int clock_configure_gpll0(void)
{
	/* Keep existing GPLL0 configuration, in RUN mode @800Mhz. */
	setbits32(&gcc->gpll0.user_ctl,
			1 << CLK_CTL_GPLL_PLLOUT_LV_EARLY_SHFT |
			1 << CLK_CTL_GPLL_PLLOUT_AUX2_SHFT |
			1 << CLK_CTL_GPLL_PLLOUT_AUX_SHFT |
			1 << CLK_CTL_GPLL_PLLOUT_MAIN_SHFT);
	return 0;
}

static int clock_configure_mnd(struct qcs405_clock *clk, uint32_t m, uint32_t n,
				uint32_t d_2)
{
	uint32_t reg_val;

	/* Configure Root Clock Generator(RCG) for Dual Edge Mode */
	reg_val = read32(&clk->rcg.cfg);
	reg_val |= (2 << CLK_CTL_CFG_MODE_SHFT);
	write32(&clk->rcg.cfg, reg_val);

	/* Set M/N/D config */
	write32(&clk->m, m & CLK_CTL_RCG_MND_BMSK);
	write32(&clk->n, ~(n-m) & CLK_CTL_RCG_MND_BMSK);
	write32(&clk->d_2, ~(d_2) & CLK_CTL_RCG_MND_BMSK);

	return 0;
}

static int clock_configure(struct qcs405_clock *clk,
				struct clock_config *clk_cfg,
				uint32_t hz, uint32_t num_perfs)
{
	uint32_t reg_val;
	uint32_t idx;

	for (idx = 0; idx < num_perfs; idx++)
		if (hz <= clk_cfg[idx].hz)
			break;

	reg_val = (clk_cfg[idx].src << CLK_CTL_CFG_SRC_SEL_SHFT) |
			(clk_cfg[idx].div << CLK_CTL_CFG_SRC_DIV_SHFT);

	/* Set clock config */
	write32(&clk->rcg.cfg, reg_val);

	if (clk_cfg[idx].m != 0)
		clock_configure_mnd(clk, clk_cfg[idx].m, clk_cfg[idx].n,
				clk_cfg[idx].d_2);

	/* Commit config to RCG*/
	setbits32(&clk->rcg.cmd, BIT(CLK_CTL_CMD_UPDATE_SHFT));

	return 0;
}

static bool clock_is_off(void *cbcr_addr)
{
	return (read32(cbcr_addr) & CLK_CTL_CBC_CLK_OFF_BMSK);
}

static int clock_enable_vote(void *cbcr_addr, void *vote_addr,
				uint32_t vote_bit)
{

	/* Set clock vote bit */
	setbits32(vote_addr, BIT(vote_bit));

	/* Ensure clock is enabled */
	while (clock_is_off(cbcr_addr));

	return 0;
}

static int clock_enable(void *cbcr_addr)
{

	/* Set clock enable bit */
	setbits32(cbcr_addr, BIT(CLK_CTL_CBC_CLK_EN_SHFT));

	/* Ensure clock is enabled */
	while (clock_is_off(cbcr_addr))
		;

	return 0;
}

static int clock_disable(void *cbcr_addr)
{

	/* Set clock enable bit */
	clrbits32(cbcr_addr, BIT(CLK_CTL_CBC_CLK_EN_SHFT));
	return 0;
}

int clock_reset_bcr(void *bcr_addr, bool reset)
{
	struct qcs405_bcr *bcr = bcr_addr;

	if (reset)
		setbits32(&bcr->bcr, BIT(CLK_CTL_BCR_BLK_ARES_SHFT));
	else
		clrbits32(&bcr->bcr, BIT(CLK_CTL_BCR_BLK_ARES_SHFT));

	return 0;
}

void clock_configure_uart(uint32_t hz)
{
	struct qcs405_clock *uart_clk = (struct qcs405_clock *)
					&gcc->blsp1_uart2_apps_clk;

	clock_configure(uart_clk, uart_cfg, hz, ARRAY_SIZE(uart_cfg));
}

void clock_configure_spi(int blsp, int qup, uint32_t hz)
{
	struct qcs405_clock *spi_clk = 0;

	if (blsp == 1) {
		switch (qup) {
		case 0:
			spi_clk = (struct qcs405_clock *)
					&gcc->blsp1_qup0_spi_clk;
			break;
		case 1:
			spi_clk = (struct qcs405_clock *)
					&gcc->blsp1_qup1_spi_clk;
			break;
		case 2:
			spi_clk = (struct qcs405_clock *)
					&gcc->blsp1_qup2_spi_clk;
			break;
		case 3:
			spi_clk = (struct qcs405_clock *)
					&gcc->blsp1_qup3_spi_clk;
			break;
		case 4:
			spi_clk = (struct qcs405_clock *)
					&gcc->blsp1_qup4_spi_clk;
			break;
		default:
			printk(BIOS_ERR, "Invalid QUP %d\n", qup);
			return;
		}
	} else if (blsp == 2) {
		spi_clk = (struct qcs405_clock *)&gcc->blsp2_qup0_spi_clk;
	} else {
		printk(BIOS_ERR, "BLSP %d not supported\n", blsp);
		return;
	}

	clock_configure(spi_clk, spi_cfg, hz, ARRAY_SIZE(spi_cfg));
}

void clock_configure_i2c(uint32_t hz)
{
	struct qcs405_clock *i2c_clk =
			(struct qcs405_clock *)&gcc->blsp1_qup1_i2c_clk;

	clock_configure(i2c_clk, i2c_cfg, hz, ARRAY_SIZE(i2c_cfg));
}

void clock_enable_uart(void)
{
	clock_enable(&gcc->blsp1_uart2_apps_cbcr);
}

void clock_disable_uart(void)
{
	clock_disable(&gcc->blsp1_uart2_apps_cbcr);
}

void clock_enable_spi(int blsp, int qup)
{
	if (blsp == 1) {
		switch (qup) {
		case 0:
			clock_enable(&gcc->blsp1_qup0_spi_apps_cbcr);
			break;
		case 1:
			clock_enable(&gcc->blsp1_qup1_spi_apps_cbcr);
			break;
		case 2:
			clock_enable(&gcc->blsp1_qup2_spi_apps_cbcr);
			break;
		case 3:
			clock_enable(&gcc->blsp1_qup3_spi_apps_cbcr);
			break;
		case 4:
			clock_enable(&gcc->blsp1_qup4_spi_apps_cbcr);
			break;
		}
	} else if (blsp == 2)
		clock_enable(&gcc->blsp2_qup0_spi_apps_cbcr);
	else
		printk(BIOS_ERR, "BLSP%d not supported\n", blsp);
}

void clock_disable_spi(int blsp, int qup)
{
	if (blsp == 1) {
		switch (qup) {
		case 0:
			clock_enable(&gcc->blsp1_qup0_spi_apps_cbcr);
			break;
		case 1:
			clock_enable(&gcc->blsp1_qup1_spi_apps_cbcr);
			break;
		case 2:
			clock_enable(&gcc->blsp1_qup2_spi_apps_cbcr);
			break;
		case 3:
			clock_enable(&gcc->blsp1_qup3_spi_apps_cbcr);
			break;
		case 4:
			clock_enable(&gcc->blsp1_qup4_spi_apps_cbcr);
			break;
		}
	} else if (blsp == 2)
		clock_enable(&gcc->blsp2_qup0_spi_apps_cbcr);
	else
		printk(BIOS_ERR, "BLSP%d not supported\n", blsp);

}

void clock_enable_i2c(void)
{
	clock_enable(&gcc->blsp1_qup1_i2c_apps_cbcr);
}

void clock_disable_i2c(void)
{
	clock_disable(&gcc->blsp1_qup1_i2c_apps_cbcr);
}

void clock_init(void)
{
	clock_configure_gpll0();
	clock_enable_vote(&gcc->blsp1_ahb_cbcr,
				&gcc->gcc_apcs_clock_branch_en_vote,
				BLSP1_AHB_CLK_ENA);

	clock_enable_vote(&gcc->blsp2_ahb_cbcr,
				&gcc->gcc_apcs_clock_branch_en_vote,
				BLSP2_AHB_CLK_ENA);
}
