/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

static const struct pad_config gpio_table[] = {
	/* A8  : PEN_GARAGE_DET_L (wake) */
	PAD_CFG_GPI_SCI(GPP_A8, NONE, DEEP, EDGE_SINGLE, NONE),
	/* A10 : FPMCU_PCH_BOOT1 */
	PAD_CFG_GPO(GPP_A10, 0, DEEP),
	/* A18 : ISH_GP0 ==> NC */
	PAD_NC(GPP_A18, NONE),
	/* A19 : ISH_GP1 ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : ISH_GP2 ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* A22 : ISH_GP4 ==> NC */
	PAD_NC(GPP_A22, NONE),
	/* A23 : ISH_GP5 ==> NC */
	PAD_NC(GPP_A23, NONE),

	/* B19 : GSPI1_CS0# ==> NC */
	PAD_NC(GPP_B19, NONE),
	/* B20 : GSPI1_CLK ==> NC */
	PAD_NC(GPP_B20, NONE),
	/* B21 : GSPI1_MISO ==> NC */
	PAD_NC(GPP_B21, NONE),
	/* B22 : GSPI1_MOSI ==> NC */
	PAD_NC(GPP_B22, NONE),

	/* C1  : SMBDATA ==> NC */
	PAD_NC(GPP_C1, NONE),
	/* C4  : TOUCHSCREEN_DIS_L */
	PAD_CFG_GPO(GPP_C4, 0, DEEP),
	/* C7  : GPP_C7 ==> Touchscreen_INT_L */
	PAD_CFG_GPI_APIC(GPP_C7, NONE, PLTRST, LEVEL, INVERT),
	/* C11 : UART0_CTS# ==> NC */
	PAD_NC(GPP_C11, NONE),
	/* C23 : UART2_CTS# ==> NC */
	PAD_NC(GPP_C23, NONE),

	/* D4  : USI_BASE_REPORT_EN */
	PAD_CFG_GPO(GPP_D4, 0, DEEP),
	/* D9  : GPP_D9 ==> EN_PP3300_DX_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_D9, 1, DEEP),
	/* D10  : GPP_D10 ==> EN_PP3300_DX_BASE_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_D10, 1, DEEP),
	/* D15 : GPP_D15 ==> TOUCHSCREEN_RST_L */
	PAD_CFG_GPO(GPP_D15, 1, DEEP),
	/* D16 : USI_INT_L */
	PAD_CFG_GPI_APIC(GPP_D16, NONE, PLTRST, LEVEL, INVERT),

	/* F0  : GPP_F0 ==> NC */
	PAD_NC(GPP_F0, NONE),
	/* F1  : GPP_F1 ==> NC */
	PAD_NC(GPP_F1, NONE),
	/* F3  : GPP_F3 ==> MEM_STRAP_3 */
	PAD_CFG_GPI(GPP_F3, NONE, PLTRST),
	/* F10 : GPP_F10 ==> MEM_STRAP_2 */
	PAD_CFG_GPI(GPP_F10, NONE, PLTRST),
	/* F11 : EMMC_CMD ==> NC */
	PAD_NC(GPP_F11, NONE),
	/* F20 : EMMC_RCLK ==> NC */
	PAD_NC(GPP_F20, NONE),
	/* F21 : EMMC_CLK ==> NC */
	PAD_NC(GPP_F21, NONE),
	/* F22 : EMMC_RESET# ==> NC */
	PAD_NC(GPP_F22, NONE),

	/* G0  : GPP_G0 ==> NC  */
	PAD_NC(GPP_G0, NONE),
	/* G1  : GPP_G1 ==> NC  */
	PAD_NC(GPP_G1, NONE),
	/* G2  : GPP_G2 ==> NC  */
	PAD_NC(GPP_G2, NONE),
	/* G3  : GPP_G3 ==> NC  */
	PAD_NC(GPP_G3, NONE),
	/* G4  : GPP_G4 ==> NC  */
	PAD_NC(GPP_G4, NONE),
	/* G5  : GPP_G5 ==> NC  */
	PAD_NC(GPP_G5, NONE),
	/* G6  : GPP_G6 ==> NC  */
	PAD_NC(GPP_G6, NONE),

	/* H3  : SPKR_PA_EN */
	PAD_CFG_GPO(GPP_H3, 1, DEEP),
	/* H4  : Touchscreen I2C2_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5  : Touchscreen I2C2_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* H13 : M2_SKT2_CFG1 ==> SPKR_RST_L */
	PAD_CFG_GPO(GPP_H13, 1, DEEP),
	/* H14 : M2_SKT2_CFG2 ==> TOUCHSCREEN_STOP_L */
	PAD_CFG_GPO(GPP_H14, 1, PLTRST),
	/* H19 : TIMESYNC[0] ==> MEM_STRAP_0 */
	PAD_CFG_GPI(GPP_H19, NONE, PLTRST),
	/* H22 : MEM_STRAP_1 */
	PAD_CFG_GPI(GPP_H22, NONE, PLTRST),
};

const struct pad_config *override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/*
 * GPIOs configured before ramstage
 * Note: the Hatch platform's romstage will configure
 * the MEM_STRAP_* (a.k.a GPIO_MEM_CONFIG_*) pins
 * as inputs before it reads them, so they are not
 * needed in this table.
 */
static const struct pad_config early_gpio_table[] = {
	/* B15 : H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* B16 : H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* B17 : H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* B18 : H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
	/* C8  : UART_PCH_RX_DEBUG_TX */
	PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1),
	/* C9  : UART_PCH_TX_DEBUG_RX */
	PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1),
	/* C14 : BT_DISABLE_L */
	PAD_CFG_GPO(GPP_C14, 0, DEEP),
	/* PCH_WP_OD */
	PAD_CFG_GPI(GPP_C20, NONE, DEEP),
	/* C21 : H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_C21, NONE, PLTRST, LEVEL, INVERT),
	/* C22 : EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_C22, NONE, DEEP),
	/* E1  : M2_SSD_PEDET */
	PAD_CFG_NF(GPP_E1, NONE, DEEP, NF1),
	/* E5  : SATA_DEVSLP1 */
	PAD_CFG_NF(GPP_E5, NONE, PLTRST, NF1),
	/* F2  : MEM_CH_SEL */
	PAD_CFG_GPI(GPP_F2, NONE, PLTRST),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* GPIOs needed to be set in romstage. */
static const struct pad_config romstage_gpio_table[] = {
	/* Enable touchscreens, hold in reset */
	/* D9  : EN_PP3300_DX_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_D9, 1, DEEP),
	/* D10 : EN_PP3300_DX_BASE_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_D10, 1, DEEP),
	/* D15 : TOUCHSCREEN_RST_L */
	PAD_CFG_GPO(GPP_D15, 0, DEEP),
};

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
