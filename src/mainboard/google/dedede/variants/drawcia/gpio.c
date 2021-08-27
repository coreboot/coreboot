/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <bootstate.h>
#include <fw_config.h>
#include <ec/google/chromeec/ec.h>

/* Pad configuration in ramstage*/
static const struct pad_config not_board6or8_gpio_table[] = {

	/* C12 : AP_PEN_DET_ODL */
	PAD_CFG_GPI_GPIO_DRIVER(GPP_C12, UP_20K, DEEP),
};

/* bid6: Pad configuration for board version 6 or 8 in ramstage*/
static const struct pad_config board6or8_gpio_table[] = {

	/* A10 : WWAN_EN */
	PAD_CFG_GPO(GPP_A10, 1, PWROK),

	/* B7  : PCIE_CLKREQ2_N ==> WWAN_SAR_DETECT_ODL*/
	PAD_CFG_GPO(GPP_B7, 1, DEEP),

	/* C12 : AP_PEN_DET_ODL has an external pull-up and hence no pad termination.*/
	PAD_CFG_GPI_GPIO_DRIVER(GPP_C12, NONE, DEEP),

	/* D0  : WWAN_HOST_WAKE ==> WWAN_WDISABLE_L */
	PAD_CFG_GPO(GPP_D0, 1, DEEP),

	/* H0 : WWAN_PERST */
	PAD_CFG_GPO(GPP_H0, 0, PLTRST),
	PAD_CFG_GPO(GPP_H17, 0, PLTRST),
};

static const struct pad_config lte_disable_pads[] = {
	PAD_NC(GPP_A10, NONE),
	PAD_NC(GPP_B7, NONE),
	PAD_NC(GPP_D0, NONE),
	PAD_NC(GPP_H0, NONE),
	PAD_NC(GPP_H17, NONE),
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

static void fw_config_handle(void *unused)
{
	if (!fw_config_probe(FW_CONFIG(DB_PORTS, DB_PORTS_1A_HDMI_LTE)))
		gpio_configure_pads(lte_disable_pads, ARRAY_SIZE(lte_disable_pads));
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
