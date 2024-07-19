/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <soc/gpio.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {
	/* A14 : USB_OC1# ==> USB_A0_FAULT_ODL */
	PAD_CFG_NF_LOCK(GPP_A14, NONE, NF1, LOCK_CONFIG),
	/* A15 : USB_OC2# ==> USB_A1_FAULT_ODL */
	PAD_CFG_NF_LOCK(GPP_A15, NONE, NF1, LOCK_CONFIG),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* A17 : GPP_A17 ==> GSC_SOC_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A17, NONE, PLTRST, LEVEL, INVERT),
	/* E3 : PROC_GP0 ==> SOC_WP_OD */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_E3, NONE, DEEP),
	/* F18 : THC1_SPI2_INT# ==> EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_F18, NONE, DEEP),
	/* H4  : I2C0_SDA ==> SOC_I2C_GSC_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5  : I2C0_SCL ==> SOC_I2C_GSC_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* H10 : UART0_RXD ==> UART_SOC_RX_DBG_TX */
	PAD_CFG_NF(GPP_H10, NONE, DEEP, NF2),
	/* H11 : UART0_TXD ==> UART_SOC_TX_DBG_RX */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF2),
};

/* Fill romstage gpio configuration */
static const struct pad_config romstage_gpio_table[] = {
	/* Enable touchscreen, hold in reset */
	/* E17  : GPP_E17 ==> SOC_TS_PWR_EN */
	PAD_CFG_GPO(GPP_E17, 1, DEEP),
	/* D15  : GPP_D15 ==> SOC_TS_I2C_RST# */
	PAD_CFG_GPO(GPP_D15, 0, DEEP),
};

const struct pad_config *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	*num = 0;
	return NULL;
}

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(GPIO_PCH_WP, CROS_GPIO_DEVICE_NAME),
};
DECLARE_CROS_GPIOS(cros_gpios);

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
