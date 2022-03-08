/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <gpio.h>
#include <soc/gpio.h>

/* This table is used by nipperkin variant with board version < 2. */
static const struct soc_amd_gpio bid1_override_gpio_table[] = {
	/* Unused TP1056 */
	PAD_NC(GPIO_4),
	/* Unused TP1063 */
	PAD_NC(GPIO_17),
	PAD_NC(GPIO_18),
	/* LCD_PRIVACY_PCH */
	PAD_GPO(GPIO_5, HIGH),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_3, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* Unused */
	PAD_NC(GPIO_85),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_32, LOW),
	/* EN_SPKR */
	PAD_GPO(GPIO_31, HIGH),
	/* Unused TP27 */
	PAD_NC(GPIO_70),
};

/* This table is used by nipperkin variant with board version >= 2. */
static const struct soc_amd_gpio bid2_override_gpio_table[] = {
	/* Unused TP1056 */
	PAD_NC(GPIO_4),
	/* Unused TP1063 */
	PAD_NC(GPIO_17),
	/* LCD_PRIVACY_PCH */
	PAD_GPO(GPIO_18, HIGH),
	/* SOC_SC_PWRSV */
	PAD_GPO(GPIO_31, HIGH),
	/* WWAN_DPR_SAR_ODL */
	PAD_GPO(GPIO_42, HIGH),
};

static const struct soc_amd_gpio override_early_gpio_table[] = {
	PAD_NC(GPIO_18),
	/* BID==1: EN_SPKR to select RAM_ID input, BID >= 1: Unused in later stages */
	PAD_GPO(GPIO_31, LOW),
};

static const struct soc_amd_gpio override_pcie_gpio_table[] = {
	PAD_NC(GPIO_18),
};


/* This table is used by nipperkin variant with board version < 2. */
static const struct soc_amd_gpio bid1_tpm_gpio_table[] = {
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_3, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
};

/* This table is used by nipperkin variant with board version >= 2. */
static const struct soc_amd_gpio bid2_tpm_gpio_table[] = {
	/* I2C3_SCL */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* I2C3_SDA */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_85, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
};

const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	uint32_t board_version = board_id();

	if (board_version < 2) {
		*size = ARRAY_SIZE(bid1_override_gpio_table);
		return bid1_override_gpio_table;
	}

	*size = ARRAY_SIZE(bid2_override_gpio_table);
	return bid2_override_gpio_table;
}

const struct soc_amd_gpio *variant_early_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(override_early_gpio_table);
	return override_early_gpio_table;
}

const struct soc_amd_gpio *variant_pcie_override_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(override_pcie_gpio_table);
	return override_pcie_gpio_table;
}

const struct soc_amd_gpio *variant_tpm_gpio_table(size_t *size)
{
	uint32_t board_version = board_id();

	if (board_version < 2) {
		*size = ARRAY_SIZE(bid1_tpm_gpio_table);
		return bid1_tpm_gpio_table;
	}

	*size = ARRAY_SIZE(bid2_tpm_gpio_table);
	return bid2_tpm_gpio_table;
}
