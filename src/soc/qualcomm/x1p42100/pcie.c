/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/qcom_qmp_phy.h>
#include <soc/pcie.h>

#define NVME_REG_EN GPIO(18)

static const struct qcom_qmp_phy_init_tbl x1p42100_qmp_pcie_serdes_pll_tbl[] = {
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_SSC_STEP_SIZE1_MODE1, 0x26),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_SSC_STEP_SIZE2_MODE1, 0x03),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_CP_CTRL_MODE1, 0x06),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_PLL_RCTRL_MODE1, 0x16),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_PLL_CCTRL_MODE1, 0x36),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_CORECLK_DIV_MODE1, 0x04),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_LOCK_CMP1_MODE1, 0x0A),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_LOCK_CMP2_MODE1, 0x1A),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_DEC_START_MODE1, 0x68),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_DIV_FRAC_START1_MODE1, 0xAB),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_DIV_FRAC_START2_MODE1, 0xAA),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_DIV_FRAC_START3_MODE1, 0x02),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_HSCLK_SEL_1, 0x12),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_SSC_STEP_SIZE1_MODE0, 0xF8),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_SSC_STEP_SIZE2_MODE0, 0x01),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_CP_CTRL_MODE0, 0x06),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_PLL_RCTRL_MODE0, 0x16),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_PLL_CCTRL_MODE0, 0x36),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_CORECLK_DIV_MODE0, 0x0A),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_LOCK_CMP1_MODE0, 0x04),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_LOCK_CMP2_MODE0, 0x0D),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_DEC_START_MODE0, 0x41),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_DIV_FRAC_START1_MODE0, 0xAB),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_DIV_FRAC_START2_MODE0, 0xAA),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_DIV_FRAC_START3_MODE0, 0x01),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_HSCLK_HS_SWITCH_SEL_1, 0x00),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_BG_TIMER, 0x0A),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_SSC_EN_CENTER, 0x01),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_SSC_PER1, 0x62),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_SSC_PER2, 0x02),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_POST_DIV_MUX, 0x40),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_CLK_ENABLE1, 0x90),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_SYS_CLK_CTRL, 0x82),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_PLL_IVCO, 0x0f),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_SYSCLK_EN_SEL, 0x08),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_LOCK_CMP_EN, 0x46),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_LOCK_CMP_CFG, 0x04),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_VCO_TUNE_MAP, 0x14),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_CLK_SELECT, 0x34),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_CORE_CLK_EN, 0xA0),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_CMN_CONFIG_1, 0x06),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_CMN_MISC1, 0x88),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_CMN_MODE, 0x14),
	QMP_PHY_INIT_CFG(QSERDES_V4_PLL_VCO_DC_LEVEL_CTRL, 0x0F),
};

static const struct qcom_qmp_phy_init_tbl x1p42100_qmp_pcie_tx_tbl[] = {
	QMP_PHY_INIT_CFG(QSERDES_V4_TX_RES_CODE_LANE_OFFSET_TX, 0x1D),
	QMP_PHY_INIT_CFG(QSERDES_V4_TX_RES_CODE_LANE_OFFSET_RX, 0x03),
	QMP_PHY_INIT_CFG(QSERDES_V4_TX_LANE_MODE_1, 0x01),
	QMP_PHY_INIT_CFG(QSERDES_V4_TX_LANE_MODE_2, 0x10),
	QMP_PHY_INIT_CFG(QSERDES_V4_TX_LANE_MODE_3, 0x90),
	QMP_PHY_INIT_CFG(QSERDES_V4_TX_TRAN_DRVR_EMP_EN, 0x34),
};

static const struct qcom_qmp_phy_init_tbl x1p42100_qmp_pcie_rx_tbl[] = {
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_UCDR_FO_GAIN_RATE2, 0x0C),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_UCDR_SO_GAIN_RATE2, 0x04),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_UCDR_FO_GAIN_RATE3, 0x0A),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_UCDR_SO_GAIN_RATE3, 0x05),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_UCDR_PI_CONTROLS, 0x16),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_UCDR_SO_ACC_DEFAULT_VAL_RATE3, 0x00),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_IVCM_CAL_CTRL2, 0x82),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_IVCM_POSTCAL_OFFSET, 0x00),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_BKUP_CTRL1, 0x12),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_DFE_1, 0x01),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_DFE_2, 0x01),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_DFE_3, 0x45),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_VGA_CAL_MAN_VAL, 0x0A),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_VGA_CAL_CNTRL1, 0x00),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_GM_CAL, 0x0D),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_EQU_ADAPTOR_CNTRL4, 0x0B),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_SIGDET_ENABLES, 0x1C),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_PHPRE_CTRL, 0x20),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_DFE_CTLE_POST_CAL_OFFSET, 0x38),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_Q_PI_INTRINSIC_BIAS_RATE32, 0x01),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE2_B0, 0x14),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE2_B1, 0xB3),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE2_B2, 0x58),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE2_B3, 0x9A),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE2_B4, 0x28),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE2_B5, 0xB6),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE2_B6, 0xEE),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE3_B0, 0xE4),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE3_B1, 0xA4),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE3_B2, 0x60),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE3_B3, 0xDF),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE3_B4, 0x4B),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE3_B5, 0x76),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_RATE3_B6, 0xFF),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_TX_ADPT_CTRL, 0x10),
};

static const struct qcom_qmp_phy_init_tbl x1p42100_qmp_pcie_ln_shrd_tbl[] = {
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RXCLK_DIV2_CTRL, 0x01),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_DFE_DAC_ENABLE1, 0x88),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_TX_ADAPT_POST_THRESH1, 0x02),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_TX_ADAPT_POST_THRESH2, 0x0D),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MODE_RATE_0_1_B0, 0xD4),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MODE_RATE_0_1_B1, 0x12),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MODE_RATE_0_1_B2, 0xDB),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MODE_RATE_0_1_B3, 0x9A),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MODE_RATE_0_1_B4, 0x32),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MODE_RATE_0_1_B5, 0xB6),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MODE_RATE_0_1_B6, 0x64),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MARG_COARSE_THRESH1_RATE210, 0x1F),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MARG_COARSE_THRESH1_RATE3, 0x1F),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MARG_COARSE_THRESH2_RATE210, 0x1F),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MARG_COARSE_THRESH2_RATE3, 0x1F),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MARG_COARSE_THRESH3_RATE210, 0x1F),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MARG_COARSE_THRESH3_RATE3, 0x1F),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MARG_COARSE_THRESH4_RATE3, 0x1F),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MARG_COARSE_THRESH5_RATE3, 0x1F),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_MARG_COARSE_THRESH6_RATE3, 0x1F),
	QMP_PHY_INIT_CFG(QSERDES_V4_LN_SHRD_RX_SUMMER_CAL_SPD_MODE, 0x5B),
};

static const struct qcom_qmp_phy_init_tbl x1p42100_qmp_pcie_pcs_com_tbl[] = {
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_COM_G3S2_PRE_GAIN, 0x2E),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_COM_RX_SIGDET_LVL, 0xAA),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_COM_EQ_CONFIG4, 0x00),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_COM_EQ_CONFIG5, 0x22),
};

static const struct qcom_qmp_phy_init_tbl x1p42100_qmp_pcie_pcs_pcie_tbl[] = {
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_ENDPOINT_REFCLK_DRIVE, 0xc1),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_OSC_DTCT_ACTIONS, 0x00),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_EQ_CONFIG1, 0x16),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_G4_EQ_CONFIG5, 0x02),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_G4_PRE_GAIN, 0x2E),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_RX_MARGINING_CONFIG1, 0x03),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_RX_MARGINING_CONFIG3, 0x28),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_RX_MARGINING_CONFIG5, 0x18),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_G3_FOM_EQ_CONFIG5, 0x7A),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_G4_FOM_EQ_CONFIG5, 0x8A),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_G3_RXEQEVAL_TIME, 0x27),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_G4_RXEQEVAL_TIME, 0x27),
};

static const struct qcom_qmp_phy_init_tbl x1p42100_qmp_gen4x4_pcie_com_tx_rx_tbl[] = {
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_COM_PCS_TX_RX_CONFIG, 0x04),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_COM_PCS_TX_RX_CONFIG2, 0x02),
};

static const struct qcom_qmp_phy_init_tbl x1p42100_qmp_gen4x4_pcie_pcs_tx_rx_tbl[] = {
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_PCS_TX_RX_CONFIG, 0xC0),
};

static const struct qcom_qmp_phy_init_tbl x1p42100_qmp_gen4x4_pcie_power_misc_tbl[] = {
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_PCIE_POWER_STATE_CONFIG2, 0x1D),
};

static const struct qcom_qmp_phy_init_tbl x1p42100_qmp_gen4x4_lane0_pcie_pcs_tbl[] = {
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_LANE0_OUTSIG_MX_CTRL2, 0x00),
};

static const struct qcom_qmp_phy_init_tbl x1p42100_qmp_gen4x4_lane1_pcie_pcs_tbl[] = {
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_LANE1_OUTSIG_MX_CTRL2, 0x00),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_LANE1_INSIG_SW_CTRL2, 0x00),
	QMP_PHY_INIT_CFG(QPHY_PCIE4_PCS_LANE1_INSIG_MX_CTRL2, 0x00),
};

static pcie_cntlr_cfg_t pcie_host = {
	.parf           = (void *)PCIE6A_PCIE_PARF,
	.dbi_base       = (void *)PCIE6A_GEN1X4_PCIE_DBI,
	.elbi           = (void *)PCIE6A_GEN1X4_PCIE_ELBI,
	.atu_base       = (void *)PCIE6A_GEN1X4_DWC_PCIE_DM_IATU,
	.cfg_base       = (void *)PCIE6A_GEN1X4_PCIE_DBI + PCIE_EP_CONF_OFFSET,
	.pcie_bcr       = (void *)PCIE6A_BCR,
	.qmp_phy_bcr    = (void *)GCC_PCIE_6A_PHY_BCR,
	.lanes          = PCIE_1x4_NUM_LANES,
	.cfg_size       = PCIE_EP_CONF_SIZE,
	.perst          = GPIO(152),

	/* Store the IO and MEM space settings for future use by the ATU */
	.io.phys_start  = PCIE6A_GEN1X4_PCIE_DBI + PCIE_IO_SPACE_OFFSET,
	.io.size        = PCIE_IO_SPACE_SIZE,
	.mem.phys_start = PCIE6A_GEN1X4_PCIE_DBI + PCIE_MMIO_SPACE_OFFSET,
	.mem.size       = PCIE6A_SPACE_END_ADDR,
};

static pcie_qmp_phy_cfg_t pcie6a_qmp_phy_1x4 = {
	.porta = {
		.qmp_phy_base      = (void *)PCIE_6A_AQMP_PHY,
		.serdes            = (void *)PCIE6A_AQPHY_SERDES,
		.tx0               = (void *)PCIE6A_AQPHY_TX0,
		.rx0               = (void *)PCIE6A_AQPHY_RX0,
		.pcs               = (void *)PCIE6A_AQPHY_PCS_COM,
		.tx1               = (void *)PCIE6A_AQPHY_TX1,
		.rx1               = (void *)PCIE6A_AQPHY_RX1,
		.pcs_misc          = (void *)PCIE6A_AQPHY_PCS_PCIE,
		.lane0_pcs         = (void *)PCIE6A_AQPHY_PCS_LANE0,
		.lane1_pcs         = (void *)PCIE6A_AQPHY_PCS_LANE1,
		.lane0_pcie_pcs    = (void *)PCIE6A_AQPHY_PCS_PCIE_LANE0,
		.lane1_pcie_pcs    = (void *)PCIE6A_AQPHY_PCS_PCIE_LANE1,
		.ln_shrd           = (void *)PCIE6A_AQPHY_LN_SHRD,
	},
	.portb = {
		.qmp_phy_base      = (void *)PCIE_6A_BQMP_PHY,
		.serdes            = (void *)PCIE6A_BQPHY_SERDES,
		.tx0               = (void *)PCIE6A_BQPHY_TX0,
		.rx0               = (void *)PCIE6A_BQPHY_RX0,
		.pcs               = (void *)PCIE6A_BQPHY_PCS_COM,
		.tx1               = (void *)PCIE6A_BQPHY_TX1,
		.rx1               = (void *)PCIE6A_BQPHY_RX1,
		.pcs_misc          = (void *)PCIE6A_BQPHY_PCS_PCIE,
		.lane0_pcs         = (void *)PCIE6A_BQPHY_PCS_LANE0,
		.lane1_pcs         = (void *)PCIE6A_BQPHY_PCS_LANE1,
		.lane0_pcie_pcs    = (void *)PCIE6A_BQPHY_PCS_PCIE_LANE0,
		.lane1_pcie_pcs    = (void *)PCIE6A_BQPHY_PCS_PCIE_LANE1,
		.ln_shrd           = (void *)PCIE6A_BQPHY_LN_SHRD,
	},
	.serdes_tbl             = x1p42100_qmp_pcie_serdes_pll_tbl,
	.serdes_tbl_num         = ARRAY_SIZE(x1p42100_qmp_pcie_serdes_pll_tbl),
	.tx_tbl                 = x1p42100_qmp_pcie_tx_tbl,
	.tx_tbl_num             = ARRAY_SIZE(x1p42100_qmp_pcie_tx_tbl),
	.rx_tbl                 = x1p42100_qmp_pcie_rx_tbl,
	.rx_tbl_num             = ARRAY_SIZE(x1p42100_qmp_pcie_rx_tbl),
	.pcs_tbl                = x1p42100_qmp_pcie_pcs_com_tbl,
	.pcs_tbl_num            = ARRAY_SIZE(x1p42100_qmp_pcie_pcs_com_tbl),
	.pcs_misc_tbl           = x1p42100_qmp_pcie_pcs_pcie_tbl,
	.pcs_misc_tbl_num       = ARRAY_SIZE(x1p42100_qmp_pcie_pcs_pcie_tbl),
	.pcs_tbl_sec            = x1p42100_qmp_gen4x4_pcie_com_tx_rx_tbl,
	.pcs_tbl_num_sec        = ARRAY_SIZE(x1p42100_qmp_gen4x4_pcie_com_tx_rx_tbl),
	.pcs_misc_tbl_sec       = x1p42100_qmp_gen4x4_pcie_pcs_tx_rx_tbl,
	.pcs_misc_tbl_num_sec   = ARRAY_SIZE(x1p42100_qmp_gen4x4_pcie_pcs_tx_rx_tbl),
	.lane0_pcie_pcs_tbl     = x1p42100_qmp_gen4x4_lane0_pcie_pcs_tbl,
	.lane0_pcie_pcs_tbl_num = ARRAY_SIZE(x1p42100_qmp_gen4x4_lane0_pcie_pcs_tbl),
	.lane1_pcie_pcs_tbl     = x1p42100_qmp_gen4x4_lane1_pcie_pcs_tbl,
	.lane1_pcie_pcs_tbl_num = ARRAY_SIZE(x1p42100_qmp_gen4x4_lane1_pcie_pcs_tbl),
	.pcs_misc_tbl_thrd      = x1p42100_qmp_gen4x4_pcie_power_misc_tbl,
	.pcs_misc_tbl_num_thrd  = ARRAY_SIZE(x1p42100_qmp_gen4x4_pcie_power_misc_tbl),
	.ln_shrd_tbl            = x1p42100_qmp_pcie_ln_shrd_tbl,
	.ln_shrd_tbl_num        = ARRAY_SIZE(x1p42100_qmp_pcie_ln_shrd_tbl),
};

/* Enable PIPE clock */
int qcom_dw_pcie_enable_pipe_clock(void)
{
	/* Set pipe clock source */
	if (clock_configure_mux(PCIE_6A_PIPE_MUXR, PCIE_6A_PIPE_SRC_SEL)) {
		printk(BIOS_ERR, " %s(): Pipe clock enable failed\n", __func__);
		return -1;
	}

	return 0;
}

/* Enable controller specific clocks */
int32_t qcom_dw_pcie_enable_clock(void)
{
	int32_t ret, clk, gdsc;

	/* Enable gdsc before enable pcie clocks */
	for (gdsc = PCIE_6A_GDSC; gdsc < MAX_GDSC; gdsc++) {
		ret = clock_enable_gdsc(gdsc);
		if (ret) {
			printk(BIOS_ERR, "Failed to enable gdsc\n");
			return ret;
		}
	}

	/* Configure gcc_pcie*_phy_rchng_clk to 100mhz */
	clock_configure_pcie();

	/* Enable pcie and PHY clocks */
	for (clk = ANOC_PCIE_AT_CBCR; clk < PCIE_CLK_COUNT; clk++) {
		if (clk == PCIE_6A_PIPE_MUXR) {
			printk(BIOS_DEBUG, "Skipping pipe\n");
			continue;
		}
		ret = clock_enable_pcie(clk);
		if (ret) {
			printk(BIOS_ERR, "Failed to enable %d clock\n", clk);
			return ret;
		}
	}
	write32(TCSR_GCC_PCIE_4L_CLKREF_EN_PCIE_ENABLE, 0x1);
	return ret;
}

/* Turn on NVMe */
void gcom_pcie_power_on_ep(void)
{
	gpio_output(NVME_REG_EN, 1);
}

void gcom_pcie_get_config(struct qcom_pcie_cntlr_t *host_cfg)
{
	host_cfg->cntlr_cfg = &pcie_host;
	host_cfg->qmp_phy_cfg = &pcie6a_qmp_phy_1x4;
}
