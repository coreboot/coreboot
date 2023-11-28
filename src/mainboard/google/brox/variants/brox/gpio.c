/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/gpio.h>


static const struct pad_config romstage_gpio_table[] = {

	/* GPP_F9 : [NF1: BOOTMPC NF6: USB_C_GPP_F9] ==> SSD_PERST_L */
	PAD_CFG_GPO(GPP_F9, 1, DEEP),
};

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	*num = 0;
	return NULL;
}

const struct pad_config *variant_romstage_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(romstage_gpio_table);
	return romstage_gpio_table;
}
