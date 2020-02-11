/*
 * This file is part of the coreboot project.
 *
 * Copyright 2020 The coreboot project Authors.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {
	/* GPP_A0 thru GPP_A6 come configured out of reset, do not touch */
	/* A0  : ESPI_IO0 */
	/* A1  : ESPI_IO1 */
	/* A2  : ESPI_IO2 */
	/* A3  : ESPI_IO3 */
	/* A4  : ESPI_CS# */
	/* A5  : ESPI_CLK */
	/* A6  : ESPI_RESET_L */

	/* B4  : H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_B4, NONE, PLTRST, LEVEL, INVERT),
	/* B15 : H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* B16 : H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* B17 : H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* B18 : H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),

	/* C16 : AP_I2C_TRACKPAD_SDA_3V3 */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* C17 : AP_I2C_TRACKPAD_SCL_3V3 */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* C18 : AP_I2C_EMR_SDA */
	PAD_CFG_NF(GPP_C18, NONE, DEEP, NF1),
	/* C19 : AP_I2C_EMR_SCL */
	PAD_CFG_NF(GPP_C19, NONE, DEEP, NF1),
	/* C20 : UART_DBG_TX_AP_RX */
	PAD_CFG_NF(GPP_C20, NONE, DEEP, NF1),
	/* C21 : UART_AP_TX_DBG_RX */
	PAD_CFG_NF(GPP_C21, NONE, DEEP, NF1),
	/* C22 : UART2_RTS_N */
	PAD_NC(GPP_C22, DN_20K),
	/* C23 : UART2_CTS_N */
	PAD_NC(GPP_C23, DN_20K),

	/* H4  : AP_I2C_TS_SDA */
	PAD_CFG_NF(GPP_H4, NONE, DEEP, NF1),
	/* H5  : AP_I2C_TS_SCL */
	PAD_CFG_NF(GPP_H5, NONE, DEEP, NF1),
	/* H6  : AP_I2C_CAM_SDA */
	PAD_CFG_NF(GPP_H6, NONE, DEEP, NF1),
	/* H7  : AP_I2C_CAM_SCL */
	PAD_CFG_NF(GPP_H7, NONE, DEEP, NF1),
	/* H8  : AP_I2C_AUDIO_SDA */
	PAD_CFG_NF(GPP_H8, NONE, DEEP, NF1),
	/* H9  : AP_I2C_AUDIO_SCL */
	PAD_CFG_NF(GPP_H9, NONE, DEEP, NF1),
};

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* B4 : H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_B4, NONE, PLTRST, LEVEL, INVERT),
	/* B15 : H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* B16 : H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* B17 : H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* B18 : H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
};

const struct pad_config *__weak variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

const struct pad_config *__weak variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/* GPIO settings before entering sleep. */
static const struct pad_config sleep_gpio_table[] = {
};

const struct pad_config *__weak variant_sleep_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(sleep_gpio_table);
	return sleep_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
};

const struct cros_gpio *__weak variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}
