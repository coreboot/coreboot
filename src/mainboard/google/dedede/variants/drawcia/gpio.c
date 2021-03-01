/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <fw_config.h>
#include <ec/google/chromeec/ec.h>

/* Pad configuration in ramstage*/
static const struct pad_config not_board6or8_gpio_table[] = {

	/* C12 : AP_PEN_DET_ODL */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_C12, UP_20K, DEEP),
};

/* bid6: Pad configuration for board version 6 or 8 in ramstage*/
static const struct pad_config board6or8_gpio_table[] = {

	/* C12 : AP_PEN_DET_ODL has an external pull-up and hence no pad termination.*/
	PAD_CFG_GPI_GPIO_DRIVER(GPP_C12, NONE, DEEP),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	uint32_t board_version = board_id();
	*num = ARRAY_SIZE(not_board6or8_gpio_table);

	if (board_version == 6 || board_version >= 8) {
		*num = ARRAY_SIZE(board6or8_gpio_table);
		return board6or8_gpio_table;
	}

	return not_board6or8_gpio_table;
}
