/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

static const struct pad_config gpio_table[] = {
	/* TRACKPAD_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_A21, NONE, PLTRST, LEVEL, INVERT),
	/* H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
	/* PCH_I2C_TRACKPAD_SDA */
	PAD_CFG_NF(GPP_C16, NONE, DEEP, NF1),
	/* PCH_I2C_TRACKPAD_SCL */
	PAD_CFG_NF(GPP_C17, NONE, DEEP, NF1),
	/* H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_C21, NONE, DEEP, LEVEL, INVERT),
	/* PCH_WP_OD */
	PAD_CFG_GPI(GPP_C20, NONE, DEEP),
	/* EC_IN_RW_OD */
	PAD_CFG_GPI(GPP_C22, NONE, DEEP),
	/* PCH_MEM_STRAP0 */
	PAD_CFG_GPI(GPP_F20, NONE, PLTRST),
	/* PCH_MEM_STRAP1 */
	PAD_CFG_GPI(GPP_F21, NONE, PLTRST),
	/* PCH_MEM_STRAP2 */
	PAD_CFG_GPI(GPP_F11, NONE, PLTRST),
	/* PCH_MEM_STRAP3 */
	PAD_CFG_GPI(GPP_F22, NONE, PLTRST),
};

const struct pad_config *__weak variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/* GPIOs needed prior to ramstage. */
static const struct pad_config early_gpio_table[] = {
	/* H1_SLAVE_SPI_CS_L */
	PAD_CFG_NF(GPP_B15, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_CLK */
	PAD_CFG_NF(GPP_B16, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_MISO_R */
	PAD_CFG_NF(GPP_B17, NONE, DEEP, NF1),
	/* H1_SLAVE_SPI_MOSI_R */
	PAD_CFG_NF(GPP_B18, NONE, DEEP, NF1),
	/* H1_PCH_INT_ODL */
	PAD_CFG_GPI_APIC(GPP_C21, NONE, DEEP, LEVEL, INVERT),
};

const struct pad_config *__weak variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
};

const struct cros_gpio *__weak variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}
