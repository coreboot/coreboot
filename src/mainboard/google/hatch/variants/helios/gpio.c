/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

static const struct pad_config gpio_table[] = {
	/* A8  : PEN_GARAGE_DET_L (wake) */
	PAD_CFG_GPI_SCI(GPP_A8, NONE, DEEP, EDGE_SINGLE, NONE),
	/* A10 : FPMCU_PCH_BOOT1 */
	PAD_CFG_GPO(GPP_A10, 0, DEEP),
	/* A11 : PCH_SPI_FPMCU_CS_L */
	PAD_CFG_NF(GPP_A11, NONE, DEEP, NF2),
	/* A12 : FPMCU_RST_ODL */
	PAD_CFG_GPO(GPP_A12, 0, DEEP),
	/* A18 : ISH_GP0 ==> NC */
	PAD_NC(GPP_A18, NONE),
	/* A19 : ISH_GP1 ==> NC */
	PAD_NC(GPP_A19, NONE),
	/* A20 : ISH_GP2 ==> NC */
	PAD_NC(GPP_A20, NONE),
	/* B19 : GSPI1_CS0# ==> NC */
	PAD_NC(GPP_B19, NONE),
	/* C1  : SMBDATA ==> NC */
	PAD_NC(GPP_C1, NONE),
	/* C4  : TOUCHSCREEN_DIS_L */
	PAD_CFG_GPO(GPP_C4, 0, DEEP),
	/* C6  : GPP_C6 ==> NC */
	PAD_NC(GPP_C6, NONE),
	/* C7  : GPP_C7 ==> NC */
	PAD_NC(GPP_C7, NONE),
	/* C23 : UART2_CTS# ==> NC */
	PAD_NC(GPP_C23, NONE),
	/* D5  : ISH_I2C0_SDA ==> NC */
	PAD_NC(GPP_D5, NONE),
	/* D6  : ISH_I2C0_SCL ==> NC */
	PAD_NC(GPP_D6, NONE),
	/* D7  : ISH_I2C1_SDA ==> NC */
	PAD_NC(GPP_D7, NONE),
	/* D8  : ISH_I2C1_SCL ==> NC */
	PAD_NC(GPP_D8, NONE),
	/* D9  : EN_PP3300_DX_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_D9, 1, DEEP),
	/* D10 : ISH_SPI_CLK ==> EN_PP3300_PP1800_FP */
	PAD_CFG_GPO(GPP_D10, 0, DEEP),
	/* D15 : TOUCHSCREEN_RST_L */
	PAD_CFG_GPO(GPP_D15, 1, DEEP),
	/* D16 : USI_INT_L */
	PAD_CFG_GPI_APIC(GPP_D16, NONE, PLTRST, LEVEL, INVERT),
	/* D21 : SPI1_IO2 ==> NC */
	PAD_NC(GPP_D21, NONE),
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
	/* H4  : I2C2_SDA ==> NC */
	PAD_NC(GPP_H4, NONE),
	/* H5  : I2C2_SCL ==> NC */
	PAD_NC(GPP_H5, NONE),
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

/*
 * Default GPIO settings before entering non-S5 sleep states.
 * Configure A12: FPMCU_RST_ODL as GPO before entering sleep.
 * This guarantees that A12's native3 function is disabled.
 * See https://review.coreboot.org/c/coreboot/+/32111 .
 */
static const struct pad_config default_sleep_gpio_table[] = {
	PAD_CFG_GPO(GPP_A12, 1, DEEP), /* FPMCU_RST_ODL */
};

/*
 * GPIO settings before entering S5, which are same as
 * default_sleep_gpio_table but also, turn off FPMCU.
 */
static const struct pad_config s5_sleep_gpio_table[] = {
	PAD_CFG_GPO(GPP_A12, 0, DEEP), /* FPMCU_RST_ODL */
	PAD_CFG_GPO(GPP_C11, 0, DEEP), /* PCH_FP_PWR_EN */
};

const struct pad_config *variant_sleep_gpio_table(u8 slp_typ, size_t *num)
{
	if (slp_typ == ACPI_S5) {
		*num = ARRAY_SIZE(s5_sleep_gpio_table);
		return s5_sleep_gpio_table;
	}
	*num = ARRAY_SIZE(default_sleep_gpio_table);
	return default_sleep_gpio_table;
}

/* GPIOs needed to be set in romstage. */
static const struct pad_config romstage_gpio_table[] = {
	/* Enable touchscreen, hold in reset */
	/* D9  : EN_PP3300_DX_TOUCHSCREEN */
	PAD_CFG_GPO(GPP_D9, 1, DEEP),
	/* D15 : TOUCHSCREEN_RST_L */
	PAD_CFG_GPO(GPP_D15, 0, DEEP),
};

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
