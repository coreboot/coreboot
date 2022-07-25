/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_def.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/gpio.h>
#include <soc/qcom_qmp_phy.h>
#include <soc/pcie.h>

#if CONFIG(BOARD_GOOGLE_SENOR)
#define NVME_REG_EN		GPIO(19)
#else
/* For Herobrine board and all variants */
#define NVME_REG_EN		GPIO(51)
#endif

static const struct qcom_qmp_phy_init_tbl sc7280_qmp_pcie_serdes_tbl[] = {
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_SYSCLK_EN_SEL, 0x08),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_CLK_SELECT, 0x34),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_CORECLK_DIV_MODE1, 0x08),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_PLL_IVCO, 0x0f),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_LOCK_CMP_EN, 0x42),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_VCO_TUNE1_MODE0, 0x24),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_VCO_TUNE2_MODE1, 0x03),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_VCO_TUNE1_MODE1, 0xb4),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_VCO_TUNE_MAP, 0x02),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_BIN_VCOCAL_HSCLK_SEL, 0x11),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_DEC_START_MODE0, 0x82),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_DIV_FRAC_START3_MODE0, 0x03),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_DIV_FRAC_START2_MODE0, 0x55),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_DIV_FRAC_START1_MODE0, 0x55),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_LOCK_CMP2_MODE0, 0x1a),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_LOCK_CMP1_MODE0, 0x0a),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_DEC_START_MODE1, 0x68),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_DIV_FRAC_START3_MODE1, 0x02),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_DIV_FRAC_START2_MODE1, 0xaa),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_DIV_FRAC_START1_MODE1, 0xab),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_LOCK_CMP2_MODE1, 0x34),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_LOCK_CMP1_MODE1, 0x14),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_HSCLK_SEL, 0x01),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_CP_CTRL_MODE0, 0x06),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_PLL_RCTRL_MODE0, 0x16),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_PLL_CCTRL_MODE0, 0x36),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_CP_CTRL_MODE1, 0x06),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_PLL_RCTRL_MODE1, 0x16),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_PLL_CCTRL_MODE1, 0x36),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_BIN_VCOCAL_CMP_CODE2_MODE0, 0x1e),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_BIN_VCOCAL_CMP_CODE1_MODE0, 0xca),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_BIN_VCOCAL_CMP_CODE2_MODE1, 0x18),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_BIN_VCOCAL_CMP_CODE1_MODE1, 0xa2),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_SSC_EN_CENTER, 0x01),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_SSC_PER1, 0x31),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_SSC_PER2, 0x01),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_SSC_STEP_SIZE1_MODE0, 0xde),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_SSC_STEP_SIZE2_MODE0, 0x07),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_SSC_STEP_SIZE1_MODE1, 0x4c),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_SSC_STEP_SIZE2_MODE1, 0x06),
	QMP_PHY_INIT_CFG(QSERDES_V4_COM_CLK_ENABLE1, 0x90),
};

static const struct qcom_qmp_phy_init_tbl sc7280_qmp_pcie_tx_tbl[] = {
	QMP_PHY_INIT_CFG(QSERDES_V4_TX_RCV_DETECT_LVL_2, 0x12),
	QMP_PHY_INIT_CFG(QSERDES_V4_TX_LANE_MODE_1, 0x35),
	QMP_PHY_INIT_CFG(QSERDES_V4_TX_RES_CODE_LANE_OFFSET_TX, 0x11),
};

static const struct qcom_qmp_phy_init_tbl sc7280_qmp_pcie_rx_tbl[] = {
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_UCDR_FO_GAIN, 0x0c),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_UCDR_SO_GAIN, 0x03),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_GM_CAL, 0x1b),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_IDAC_TSETTLE_HIGH, 0x00),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_IDAC_TSETTLE_LOW, 0xc0),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_AUX_DATA_TCOARSE_TFINE, 0x30),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_VGA_CAL_CNTRL1, 0x04),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_VGA_CAL_CNTRL2, 0x07),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_UCDR_SO_SATURATION_AND_ENABLE, 0x7f),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_UCDR_PI_CONTROLS, 0x70),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_EQU_ADAPTOR_CNTRL2, 0x0e),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_EQU_ADAPTOR_CNTRL3, 0x4a),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_EQU_ADAPTOR_CNTRL4, 0x0f),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_SIGDET_CNTRL, 0x03),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_SIGDET_ENABLES, 0x1c),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_SIGDET_DEGLITCH_CNTRL, 0x1e),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1, 0x17),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_10_LOW, 0xd4),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_10_HIGH, 0x54),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_10_HIGH2, 0xdb),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_10_HIGH3, 0x3b),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_10_HIGH4, 0x31),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_01_LOW, 0x24),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_00_HIGH2, 0xff),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_00_HIGH3, 0x7f),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_DCC_CTRL1, 0x0c),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_01_HIGH, 0xe4),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_01_HIGH2, 0xec),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_01_HIGH3, 0x3b),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_01_HIGH4, 0x36),
};

static const struct qcom_qmp_phy_init_tbl sc7280_qmp_pcie_pcs_tbl[] = {
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_P2U3_WAKEUP_DLY_TIME_AUXCLK_L, 0x01),
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_RX_SIGDET_LVL, 0x77),
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_RATE_SLEW_CNTRL1, 0x0b),
};

static const struct qcom_qmp_phy_init_tbl sc7280_qmp_pcie_pcs_misc_tbl[] = {
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_PCIE_OSC_DTCT_ACTIONS, 0x00),
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_PCIE_L1P1_WAKEUP_DLY_TIME_AUXCLK_L, 0x01),
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_PCIE_L1P2_WAKEUP_DLY_TIME_AUXCLK_L, 0x01),
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_PCIE_PRESET_P6_P7_PRE, 0x33),
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_PCIE_PRESET_P10_PRE, 0x00),
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_PCIE_PRESET_P10_POST, 0x58),
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_PCIE_ENDPOINT_REFCLK_DRIVE, 0xc1),
};

static const struct qcom_qmp_phy_init_tbl sc7280_qmp_gen3x2_pcie_tx_tbl[] = {
	QMP_PHY_INIT_CFG(QSERDES_V4_TX_PI_QEC_CTRL, 0x20),
};

static const struct qcom_qmp_phy_init_tbl sc7280_qmp_gen3x2_pcie_rx_tbl[] = {
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_EQU_ADAPTOR_CNTRL1, 0x04),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_00_LOW, 0xbf),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_RX_MODE_00_HIGH4, 0x15),
	QMP_PHY_INIT_CFG(QSERDES_V4_RX_DFE_CTLE_POST_CAL_OFFSET, 0x38),
};

static const struct qcom_qmp_phy_init_tbl sc7280_qmp_gen3x2_pcie_pcs_tbl[] = {
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_REFGEN_REQ_CONFIG1, 0x05),
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_EQ_CONFIG2, 0x0f),
};

static const struct qcom_qmp_phy_init_tbl sc7280_qmp_gen3x2_pcie_misc_tbl[] = {
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_PCIE_POWER_STATE_CONFIG2, 0x0d),
	QMP_PHY_INIT_CFG(QPHY_V4_PCS_PCIE_POWER_STATE_CONFIG4, 0x07),
};

static pcie_cntlr_cfg_t pcie_host = {
	.parf		= (void *) PCIE1_PCIE_PARF,
	.dbi_base	= (void *) PCIE1_GEN3X2_PCIE_DBI,
	.elbi		= (void *) PCIE1_GEN3X2_PCIE_ELBI,
	.atu_base	= (void *) PCIE1_GEN3X2_DWC_PCIE_DM_IATU,
	.cfg_base	= (void *) PCIE1_GEN3X2_PCIE_DBI + PCIE_EP_CONF_OFFSET,
	.pcie_bcr	= (void *) PCIE1_BCR,
	.qmp_phy_bcr	= (void *) GCC_PCIE_1_PHY_BCR,
	.lanes		= PCIE_3x2_NUM_LANES,
	.cfg_size	= PCIE_EP_CONF_SIZE,
	.perst		= GPIO(2),

	/* Store the IO and MEM space settings for future use by the ATU */
	.io.phys_start	= PCIE1_GEN3X2_PCIE_DBI +  PCIE_IO_SPACE_OFFSET,
	.io.size	= PCIE_IO_SPACE_SIZE,

	.mem.phys_start	= PCIE1_GEN3X2_PCIE_DBI +  PCIE_MMIO_SPACE_OFFSET,
	.mem.size	= PCIE1_SPACE_END_ADDR,
};

static pcie_qmp_phy_cfg_t pcie1_qmp_phy_3x2 = {
	.qmp_phy_base		= (void *) PCIE_1_QMP_PHY,
	.serdes			= (void *) PCE1_QPHY_SERDES,
	.tx0			= (void *) PCE1_QPHY_TX0,
	.rx0			= (void *) PCE1_QPHY_RX0,
	.pcs			= (void *) PCIE1_QMP_PHY_PCS_COM,
	.tx1			= (void *) PCE1_QPHY_TX1,
	.rx1			= (void *) PCE1_QPHY_RX1,
	.pcs_misc		= (void *) PCE1_QPHY_PCS_MISC,
	.serdes_tbl		= sc7280_qmp_pcie_serdes_tbl,
	.serdes_tbl_num		= ARRAY_SIZE(sc7280_qmp_pcie_serdes_tbl),
	.tx_tbl			= sc7280_qmp_pcie_tx_tbl,
	.tx_tbl_num		= ARRAY_SIZE(sc7280_qmp_pcie_tx_tbl),
	.tx_tbl_sec		= sc7280_qmp_gen3x2_pcie_tx_tbl,
	.tx_tbl_num_sec		= ARRAY_SIZE(sc7280_qmp_gen3x2_pcie_tx_tbl),
	.rx_tbl			= sc7280_qmp_pcie_rx_tbl,
	.rx_tbl_num		= ARRAY_SIZE(sc7280_qmp_pcie_rx_tbl),
	.rx_tbl_sec		= sc7280_qmp_gen3x2_pcie_rx_tbl,
	.rx_tbl_num_sec		= ARRAY_SIZE(sc7280_qmp_gen3x2_pcie_rx_tbl),
	.pcs_tbl		= sc7280_qmp_pcie_pcs_tbl,
	.pcs_tbl_num		= ARRAY_SIZE(sc7280_qmp_pcie_pcs_tbl),
	.pcs_tbl_sec		= sc7280_qmp_gen3x2_pcie_pcs_tbl,
	.pcs_tbl_num_sec	= ARRAY_SIZE(sc7280_qmp_gen3x2_pcie_pcs_tbl),
	.pcs_misc_tbl		= sc7280_qmp_pcie_pcs_misc_tbl,
	.pcs_misc_tbl_num	= ARRAY_SIZE(sc7280_qmp_pcie_pcs_misc_tbl),
	.pcs_misc_tbl_sec	= sc7280_qmp_gen3x2_pcie_misc_tbl,
	.pcs_misc_tbl_num_sec	= ARRAY_SIZE(sc7280_qmp_gen3x2_pcie_misc_tbl),
};

/* Enable PIPE clock */
int qcom_dw_pcie_enable_pipe_clock(void)
{
	int ret;

	/* Set pipe clock source */
	ret = clock_configure_mux(GCC_PCIE_1_PIPE_MUXR, PCIE_1_PIPE_SRC_SEL);
	if (ret) {
		printk(BIOS_ERR, " %s(): Pipe clock enable failed\n", __func__);
		return -1;
	}

	/* Enable pipe clock */
	ret = clock_enable_pcie(PCIE_1_PIPE_CLK);
	if (ret) {
		printk(BIOS_ERR, "Failed to enable pipe clock\n");
		return -1;
	}

	return ret;
}

/* Enable controller specific clocks */
int32_t qcom_dw_pcie_enable_clock(void)
{
	int32_t ret, clk;

	/* Enable gdsc before enable pcie clocks */
	ret = clock_enable_gdsc(PCIE_1_GDSC);
	if (ret) {
		printk(BIOS_ERR, "Failed to enable gdsc\n");
		return ret;
	}

	/* Enable pcie and PHY clocks */
	for (clk = PCIE_1_SLV_Q2A_AXI_CLK; clk < PCIE_CLK_COUNT - 3; clk++) {
		ret = clock_enable_pcie(clk);
		if (ret) {
			printk(BIOS_ERR, "Failed to enable %d clock\n", clk);
			return ret;
		}
	}

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
	host_cfg->qmp_phy_cfg = &pcie1_qmp_phy_3x2;
}
