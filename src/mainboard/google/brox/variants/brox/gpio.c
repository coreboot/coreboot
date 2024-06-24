/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/gpio.h>

/* Pad configuration in ramstage */
static const struct pad_config override_gpio_table[] = {
	/* GPP_F7 : [NF6: USB_C_GPP_F7] ==> NC */
	PAD_NC(GPP_F7, NONE),
	/* GPP_F16 : [NF1: GSXCLK NF3: THC1_SPI2_CS# NF4: GSPI1_CS0# NF6: USB_C_GPP_F16] ==> NC */
	PAD_NC(GPP_F16, NONE),
	/* GPP_F17 : [NF3: THC1_SPI2_RST# NF6: USB_C_GPP_F17] ==> NC */
	PAD_NC(GPP_F17, NONE),
	/* GPP_F18 : [NF3: THC1_SPI2_INT# NF6: USB_C_GPP_F18] ==> NC */
	PAD_NC(GPP_F18, NONE),
	/* GPP_H6 : [NF1: I2C1_SDA NF6: USB_C_GPP_H6] ==> NC */
	PAD_NC(GPP_H6, NONE),
	/* GPP_H7 : [NF1: I2C1_SCL NF6: USB_C_GPP_H7] ==> NC */
	PAD_NC(GPP_H7, NONE),
};

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	uint32_t board_version = board_id();

	*num = 0;
	if (board_version >= 1) {
		*num = ARRAY_SIZE(override_gpio_table);
		return override_gpio_table;
	}
	return NULL;
}
