/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* Pad configuration in ramstage*/
static const struct pad_config gpio_table[] = {
	/* A10 : WWAN_EN => LTE_PWR_OFF_ODL */
	PAD_CFG_GPO(GPP_A10, 1, PWROK),

	/* H17 : WWAN_RST_L => LTE_RESET_R_ODL */
	PAD_CFG_GPO(GPP_H17, 0, PLTRST),
};

const struct pad_config *variant_override_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
