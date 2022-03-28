/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <gpio.h>
#include <soc/gpio.h>

/* This table is used by guybrush variant with board version < 2. */
static const struct soc_amd_gpio bid1_ramstage_gpio_table[] = {
	/* EN_SPKR */
	PAD_GPO(GPIO_69, HIGH),
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_70, HIGH),
	/* RAM_ID_CHAN_SEL */
	PAD_GPI(GPIO_74, PULL_NONE),
	/* EN_PP5000_PEN */
	PAD_GPO(GPIO_5, HIGH),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_3, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* Unused */
	PAD_NC(GPIO_85),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_32, LOW),
};

/* This table is used by guybrush variant with board version >= 2. */
static const struct soc_amd_gpio bid2_ramstage_gpio_table[] = {
	/* EN_PP5000_PEN */
	PAD_GPO(GPIO_5, HIGH),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_3, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* Unused */
	PAD_NC(GPIO_85),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_32, LOW),
	/* EN_SPKR */
	PAD_GPO(GPIO_31, LOW),
	/* Unused TP27 */
	PAD_NC(GPIO_70),
};

static const struct soc_amd_gpio override_early_gpio_table[] = {
	/* BID>=2: EN_SPKR to select RAM_ID input, BID < 2: Unused in later stages */
	PAD_GPO(GPIO_31, LOW),
	/* BID == 1: SD_AUX_RESET_L */
	PAD_GPO(GPIO_70, LOW),
};

/* This table is used by guybrush variant with board version < 2. */
static const struct soc_amd_gpio bid1_pcie_gpio_table[] = {
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_70, HIGH),
};

static const struct soc_amd_gpio tpm_gpio_table[] = {
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_3, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	uint32_t board_version = board_id();
	*size = 0;

	if (board_version < 2) {
		*size = ARRAY_SIZE(bid1_ramstage_gpio_table);
		return bid1_ramstage_gpio_table;
	}

	*size = ARRAY_SIZE(bid2_ramstage_gpio_table);
	return bid2_ramstage_gpio_table;
}

const struct soc_amd_gpio *variant_early_override_gpio_table(size_t *size)
{
	/*
	 * This code is run before the EC is available to check the board ID
	 * since this is needed to work on all versions of guybrush, just enable
	 * both GPIOs and reconfigure them on later stages.
	 */
	*size = ARRAY_SIZE(override_early_gpio_table);
	return override_early_gpio_table;
}

const struct soc_amd_gpio *variant_pcie_override_gpio_table(size_t *size)
{
	uint32_t board_version = board_id();
	*size = 0;
	if (board_version < 2) {
		*size = ARRAY_SIZE(bid1_pcie_gpio_table);
		return bid1_pcie_gpio_table;
	}

	return NULL;
}

const struct soc_amd_gpio *variant_tpm_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(tpm_gpio_table);
	return tpm_gpio_table;
}
