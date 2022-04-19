/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/gpio.h>

static const struct pad_config board_id0_1_overrides[] = {
	/* B2  : VRALERT# ==> NC */
	PAD_NC(GPP_B2, NONE),
	/* B7  : ISH_12C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF_LOCK(GPP_B7, NONE, NF2, LOCK_CONFIG),
	/* B8  : ISH_12C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF_LOCK(GPP_B8, NONE, NF2, LOCK_CONFIG),
	/* B15 : TIME_SYNC0 ==> NC */
	PAD_NC(GPP_B15, NONE),
	/* C3  : SML0CLK ==> NC */
	PAD_NC(GPP_C3, NONE),
	/* C4  : SML0DATA ==> NC */
	PAD_NC(GPP_C4, NONE),
	/* D13 : ISH_UART0_RXD ==> PCH_I2C_CAM_SDA */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF3),
	/* D14 : ISH_UART0_TXD ==> PCH_I2C_CAM_SCL */
	PAD_CFG_NF(GPP_D14, NONE, DEEP, NF3),
	/* F19 : SRCCLKREQ6# ==> WWAN_SIM1_DET_OD */
	PAD_CFG_GPI(GPP_F19, UP_20K, DEEP),
	/* F20 : EXT_PWR_GATE# ==> HPS_RST_R */
	PAD_CFG_GPO(GPP_F20, 0, DEEP),
	/* F21 : EXT_PWR_GATE2# ==> WAKE_ON_WWAN_ODL */
	PAD_NC(GPP_F21, NONE),
	/* H21 : IMGCLKOUT2 ==> WLAN_INT_L */
	PAD_CFG_GPI_APIC(GPP_H21, NONE, DEEP, EDGE_SINGLE, NONE),
	/* GPD2: LAN_WAKE# ==> NC */
	PAD_NC(GPD2, NONE),
};

/* Early pad configuration in bootblock for board id < 2 */
static const struct pad_config early_gpio_table[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B7  : ISH_12C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF2),
	/* B8  : ISH_12C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF2),
	/*
	 * D1  : ISH_GP1 ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external pull-down.
	 * To ensure proper power sequencing for the FPMCU device, reset signal is driven low
	 * early on in bootblock, followed by enabling of power. Reset signal is deasserted
	 * later on in ramstage. Since reset signal is asserted in bootblock, it results in
	 * FPMCU not working after a S3 resume. This is a known issue.
	 */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* D2  : ISH_GP2 ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* E0  : SATAXPCIE0 ==> WWAN_PERST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_E0, 0, DEEP),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E16 : RSVD_TP ==> WWAN_RST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_E16, 0, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* F21 : EXT_PWR_GATE2# ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_NC(GPP_H13, UP_20K),
};

/* Early pad configuration in bootblock for board id 2 */
static const struct pad_config early_gpio_table_id2[] = {
	/* A12 : SATAXPCIE1 ==> EN_PP3300_WWAN */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* B7  : ISH_12C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_B7, NONE, DEEP, NF2),
	/* B8  : ISH_12C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_B8, NONE, DEEP, NF2),
	/*
	 * D1  : ISH_GP1 ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external pull-down.
	 * To ensure proper power sequencing for the FPMCU device, reset signal is driven low
	 * early on in bootblock, followed by enabling of power. Reset signal is deasserted
	 * later on in ramstage. Since reset signal is asserted in bootblock, it results in
	 * FPMCU not working after a S3 resume. This is a known issue.
	 */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* D2  : ISH_GP2 ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* D11 : ISH_SPI_MISO ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* E0  : SATAXPCIE0 ==> WWAN_PERST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_E0, 0, DEEP),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E16 : RSVD_TP ==> WWAN_RST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_E16, 0, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* F21 : EXT_PWR_GATE2# ==> WWAN_FCPO_L (updated in romstage) */
	PAD_CFG_GPO(GPP_F21, 0, DEEP),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_NC(GPP_H13, UP_20K),
};

/* Early pad configuration in bootblock for board id 4 */
static const struct pad_config early_gpio_table_id4[] = {
	/* A12 : SATAXPCIE1 ==> EN_PP3300_WWAN */
	PAD_CFG_GPO(GPP_A12, 1, DEEP),
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
	/* H6  : ISH_12C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : ISH_12C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/*
	 * D1  : ISH_GP1 ==> FP_RST_ODL
	 * FP_RST_ODL comes out of reset as hi-z and does not have an external
	 * pull-down. To ensure proper power sequencing for the FPMCU device,
	 * reset signal is driven low early on in bootblock, followed by
	 * enabling of power. Reset signal is deasserted later on in ramstage.
	 * Since reset signal is asserted in bootblock, it results in FPMCU not
	 * working after a S3 resume. This is a known issue.
	 */
	PAD_CFG_GPO(GPP_D1, 0, DEEP),
	/* D2  : ISH_GP2 ==> EN_FP_PWR */
	PAD_CFG_GPO(GPP_D2, 1, DEEP),
	/* D11 : ISH_SPI_MISO ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_D11, 1, DEEP),
	/* E0  : SATAXPCIE0 ==> WWAN_PERST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_E0, 0, DEEP),
	/* E13 : THC0_SPI1_IO2 ==> MEM_CH_SEL */
	PAD_CFG_GPI(GPP_E13, NONE, DEEP),
	/* E16 : RSVD_TP ==> WWAN_RST_L (updated in ramstage) */
	PAD_CFG_GPO(GPP_E16, 0, DEEP),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* F21 : EXT_PWR_GATE2# ==> WWAN_FCPO_L (updated in romstage) */
	PAD_CFG_GPO(GPP_F21, 0, DEEP),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_NC(GPP_H13, UP_20K),
};

static const struct pad_config romstage_gpio_table[] = {
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),
	/* F21 : EXT_PWR_GATE2# ==> WWAN_FCPO_L (set here for correct power sequencing) */
	PAD_CFG_GPO(GPP_F21, 1, DEEP),
};

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	const uint32_t id = board_id();
	if (id == BOARD_ID_UNKNOWN || id < 2) {
		*num = ARRAY_SIZE(board_id0_1_overrides);
		return board_id0_1_overrides;
	}

	*num = 0;
	return NULL;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	const uint32_t id = board_id();
	if (id == BOARD_ID_UNKNOWN || id < 2) {
		*num = ARRAY_SIZE(early_gpio_table);
		return early_gpio_table;
	} else if (id >= 4) {
		*num = ARRAY_SIZE(early_gpio_table_id4);
		return early_gpio_table_id4;
	}

	*num = ARRAY_SIZE(early_gpio_table_id2);
	return early_gpio_table_id2;
}

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
