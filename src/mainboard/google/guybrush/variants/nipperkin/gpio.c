/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <baseboard/variants.h>

/* This table is used by nipperkin variant with board version < 2. */
static const struct soc_amd_gpio bid1_override_gpio_table[] = {
	/* Unused TP1056 */
	PAD_NC(GPIO_4),
	/* Unused TP1063 */
	PAD_NC(GPIO_17),
	PAD_NC(GPIO_18),
	/* LCD_PRIVACY_PCH */
	PAD_GPO(GPIO_5, HIGH),
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_69, HIGH),
	/* GSC_SOC_INT_L */
	PAD_INT(GPIO_3, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	/* Unused */
	PAD_NC(GPIO_85),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_32, HIGH),
};

/* This table is used by nipperkin variant with board version >= 2. */
static const struct soc_amd_gpio bid2_override_gpio_table[] = {
	/* Unused TP1056 */
	PAD_NC(GPIO_4),
	/* Unused TP1063 */
	PAD_NC(GPIO_17),
	/* LCD_PRIVACY_PCH */
	PAD_GPO(GPIO_18, HIGH),
	/* Unused */
	PAD_NC(GPIO_69),
};

static const struct soc_amd_gpio override_early_gpio_table[] = {
	/* BID == 1: GSC_SOC_INT_L, BID > 1: Unused */
	PAD_INT(GPIO_3, PULL_NONE, EDGE_LOW, STATUS_DELIVERY),
	PAD_NC(GPIO_18),
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_69, LOW),
};

/* This table is used by nipperkin variant with board version < 2. */
static const struct soc_amd_gpio bid1_override_pcie_gpio_table[] = {
	PAD_NC(GPIO_5),
	PAD_NC(GPIO_18),
	/* SD_AUX_RESET_L */
	PAD_GPO(GPIO_69, HIGH),
};

/* This table is used by nipperkin variant with board version >= 2. */
static const struct soc_amd_gpio bid2_override_pcie_gpio_table[] = {
	PAD_NC(GPIO_18),
	PAD_NC(GPIO_69),
};

/* This table is used by nipperkin variant with board version < 2. */
static const struct soc_amd_gpio bid1_fpmcu_shutdown_gpio_table[] = {
	/* FPMCU_RST_L */
	PAD_GPO(GPIO_11, LOW),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_32, LOW),
};

/* This table is used by nipperkin variant with board version >= 2. */
static const struct soc_amd_gpio bid2_fpmcu_shutdown_gpio_table[] = {
	/* FPMCU_RST_L */
	PAD_GPO(GPIO_11, LOW),
	/* EN_PWR_FP */
	PAD_GPO(GPIO_3, LOW),
};

/* This table is used by nipperkin variant with board version < 2. */
static const struct soc_amd_gpio bid1_fpmcu_disable_gpio_table[] = {
	/* FPMCU_RST_L */
	PAD_NC(GPIO_11),
	/* EN_PWR_FP */
	PAD_NC(GPIO_32),
};

/* This table is used by nipperkin variant with board version >= 2. */
static const struct soc_amd_gpio bid2_fpmcu_disable_gpio_table[] = {
	/* FPMCU_RST_L */
	PAD_NC(GPIO_11),
	/* EN_PWR_FP */
	PAD_NC(GPIO_3),
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
	uint32_t board_version = board_id();

	if (board_version < 2) {
		*size = ARRAY_SIZE(bid1_override_pcie_gpio_table);
		return bid1_override_pcie_gpio_table;
	}

	*size = ARRAY_SIZE(bid2_override_pcie_gpio_table);
	return bid2_override_pcie_gpio_table;
}

const struct soc_amd_gpio *variant_fpmcu_shutdown_gpio_table(size_t *size)
{
	uint32_t board_version = board_id();

	if (board_version < 2) {
		*size = ARRAY_SIZE(bid1_fpmcu_shutdown_gpio_table);
		return bid1_fpmcu_shutdown_gpio_table;
	}

	*size = ARRAY_SIZE(bid2_fpmcu_shutdown_gpio_table);
	return bid2_fpmcu_shutdown_gpio_table;
}

const struct soc_amd_gpio *variant_fpmcu_disable_gpio_table(size_t *size)
{
	uint32_t board_version = board_id();

	if (board_version < 2) {
		*size = ARRAY_SIZE(bid1_fpmcu_disable_gpio_table);
		return bid1_fpmcu_disable_gpio_table;
	}

	*size = ARRAY_SIZE(bid2_fpmcu_disable_gpio_table);
	return bid2_fpmcu_disable_gpio_table;
}
