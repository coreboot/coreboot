/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <soc/gpio.h>
#include <boardid.h>
#include <gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* A14 : USB_OC1# ==> NC */
	PAD_NC(GPP_A14, NONE),
	/* A19 : DDSP_HPD1 ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : DDSP_HPD2 ==> DDIC_DP_HPD4 */
	PAD_CFG_NF(GPP_A20, NONE, DEEP, NF1),
	/* A21 : DDPC_CTRCLK ==> DDIC_DP_CTRCLK */
	PAD_CFG_NF(GPP_A21, NONE, DEEP, NF1),
	/* A22 : DDPC_CTRLDATA ==> DDIC_DP_CTRLDATA */
	PAD_CFG_NF(GPP_A22, NONE, DEEP, NF1),

	/* B2  : VRALERT# ==> NC */
	PAD_NC(GPP_B2, NONE),
	/* B3  : PROC_GP2 ==> EMMC_PERST_L */
	PAD_CFG_GPO(GPP_B3, 1, DEEP),

	/* D6  : SRCCLKREQ1# ==> EMMC_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),
	/* D14 : ISH_UART0_TXD ==> USB_A1_RT_RST_ODL */
	PAD_CFG_GPO(GPP_D14, 1, DEEP),

	/* E1  : THC0_SPI1_IO2 ==> B2B_HDMICARD_DETN */
	PAD_CFG_GPI(GPP_E1, NONE, DEEP),
	/* E2  : THC0_SPI1_IO3 ==> B2B_DPCARD_DETN */
	PAD_CFG_GPI(GPP_E2, NONE, DEEP),
	/* E20 : DDP2_CTRLCLK ==> EN_PP3300_EMMC */
	PAD_CFG_GPO(GPP_E20, 1, DEEP),
	/* E21 : DDP2_CTRLDATA ==> NC */
	PAD_NC(GPP_E21, NONE),

	/* H19 : SRCCLKREQ4# ==> LAN_I225V_CLKREQ_ODL */
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1),

	/* R6 : I2S2_TXD ==> NC */
	PAD_NC(GPP_R6, NONE),
	/* R7 : I2S2_RXD ==> NC */
	PAD_NC(GPP_R7, NONE),

	/* GPD11: LANPHYC ==> LAN_DISABLE_N */
	PAD_CFG_GPO(GPD11, 0, DEEP),
};


static const struct pad_config override_gpio_table_id2[] = {
	/* A14 : USB_OC1# ==> NC */
	PAD_NC(GPP_A14, NONE),
	/* A19 : DDSP_HPD1 ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : DDSP_HPD2 ==> DDIC_DP_HPD4 */
	PAD_CFG_NF(GPP_A20, NONE, DEEP, NF1),
	/* A21 : DDP2_CTRLCLK ==> EN_PP3300_EMMC */
	PAD_CFG_GPO(GPP_A21, 1, DEEP),
	/* A22 : DDPC_CTRLDATA ==> NC */
	PAD_NC(GPP_A22, NONE),

	/* B2  : VRALERT# ==> NC */
	PAD_NC(GPP_B2, NONE),
	/* B3  : PROC_GP2 ==> EMMC_PERST_L */
	PAD_CFG_GPO(GPP_B3, 1, DEEP),

	/* D6  : SRCCLKREQ1# ==> EMMC_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),
	/* D14 : ISH_UART0_TXD ==> USB_A1_RT_RST_ODL */
	PAD_CFG_GPO(GPP_D14, 1, DEEP),

	/* E1  : THC0_SPI1_IO2 ==> B2B_HDMICARD_DETN */
	PAD_CFG_GPI(GPP_E1, NONE, DEEP),
	/* E2  : THC0_SPI1_IO3 ==> B2B_DPCARD_DETN */
	PAD_CFG_GPI(GPP_E2, NONE, DEEP),
	/* E20 : DDP2_CTRLCLK ==> DDIC_DP_CTRCLK */
	PAD_CFG_NF(GPP_E20, NONE, DEEP, NF1),
	/* E21 : DDP2_CTRLDATA ==> DDIC_DP_CTRLDATA */
	PAD_CFG_NF(GPP_E21, NONE, DEEP, NF1),

	/* H19 : SRCCLKREQ4# ==> LAN_I225V_CLKREQ_ODL */
	PAD_CFG_NF(GPP_H19, NONE, DEEP, NF1),

	/* R6 : I2S2_TXD ==> NC */
	PAD_NC(GPP_R6, NONE),
	/* R7 : I2S2_RXD ==> NC */
	PAD_NC(GPP_R7, NONE),

	/* GPD11: LANPHYC ==> LAN_DISABLE_N */
	PAD_CFG_GPO(GPD11, 0, DEEP),
};


/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* B3  : PROC_GP2 ==> EMMC_PERST_L */
	PAD_CFG_GPO(GPP_B3, 0, DEEP),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
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
	/* D18 : UART1_TXD ==> SD_PE_RST_L */
	PAD_CFG_GPO(GPP_D18, 0, PLTRST),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* E20 : DDP2_CTRLCLK ==> EN_PP3300_EMMC */
	PAD_CFG_GPO(GPP_E20, 1, DEEP),
	/* F14 : GSXDIN ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_F14, 1, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),

	/* CPU PCIe VGPIO for PEG60 */
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_48, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_49, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_50, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_51, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_52, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_53, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_54, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_55, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_56, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_57, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_58, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_59, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_60, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_61, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_62, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_63, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_76, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_77, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_78, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_79, NONE, PLTRST, NF1),
};


/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table_id2[] = {
	/* A13 : PMC_I2C_SCL ==> GSC_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A13, NONE, PLTRST, LEVEL, INVERT),
	/* A21 : DDP2_CTRLCLK ==> EN_PP3300_EMMC */
	PAD_CFG_GPO(GPP_A21, 1, DEEP),
	/* B3  : PROC_GP2 ==> EMMC_PERST_L */
	PAD_CFG_GPO(GPP_B3, 0, DEEP),
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 0, DEEP),
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
	/* D18 : UART1_TXD ==> SD_PE_RST_L */
	PAD_CFG_GPO(GPP_D18, 0, PLTRST),
	/* E15 : RSVD_TP ==> PCH_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E15, NONE, DEEP),
	/* F14 : GSXDIN ==> EN_PP3300_SSD */
	PAD_CFG_GPO(GPP_F14, 1, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H6  : I2C1_SDA ==> PCH_I2C_TPM_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : I2C1_SCL ==> PCH_I2C_TPM_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_PCH_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_PCH_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
	/* H13 : I2C7_SCL ==> EN_PP3300_SD */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),

	/* CPU PCIe VGPIO for PEG60 */
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_48, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_49, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_50, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_51, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_52, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_53, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_54, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_55, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_56, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_57, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_58, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_59, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_60, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_61, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_62, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_63, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_76, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_77, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_78, NONE, PLTRST, NF1),
	PAD_CFG_NF_VWEN(GPP_vGPIO_PCIE_79, NONE, PLTRST, NF1),
};
static const struct pad_config romstage_gpio_table[] = {
	/* B4  : PROC_GP3 ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_B4, 1, DEEP),
	/* D18 : UART1_TXD ==> SD_PE_RST_L */
	PAD_CFG_GPO(GPP_D18, 1, DEEP),
};

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	const uint32_t board_ver = board_id();
	if (board_ver <= 1) {
		*num = ARRAY_SIZE(override_gpio_table);
		return override_gpio_table;
	}
	*num = ARRAY_SIZE(override_gpio_table_id2);
	return override_gpio_table_id2;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	const uint32_t board_ver = board_id();
	if (board_ver <= 1) {
		*num = ARRAY_SIZE(early_gpio_table);
		return early_gpio_table;
	}
	*num = ARRAY_SIZE(early_gpio_table_id2);
	return early_gpio_table_id2;
}

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
