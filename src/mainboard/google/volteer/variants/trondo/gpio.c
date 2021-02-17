/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <commonlib/helpers.h>

/* Pad configuration in ramstage */
static const struct pad_config gpio_table[] = {

};

const struct pad_config *variant_base_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}

/* Early pad configuration in bootblock */
static const struct pad_config early_gpio_table[] = {
	/* C8 : UART0 RX */
	PAD_CFG_NF(GPP_C8, NONE, DEEP, NF1),
	/* C9 : UART0 TX */
	PAD_CFG_NF(GPP_C9, NONE, DEEP, NF1),
};

const struct pad_config *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

static const struct cros_gpio cros_gpios[] = {
};

const struct cros_gpio *variant_cros_gpios(size_t *num)
{
	*num = ARRAY_SIZE(cros_gpios);
	return cros_gpios;
}

/* GPIO settings before entering S5 */
static const struct pad_config s5_sleep_gpio_table[] = {
	PAD_CFG_GPO(GPP_C23, 0, DEEP), /* FPMCU_RST_ODL */
	PAD_CFG_GPO(GPP_A21, 0, DEEP), /* EN_FP_PWR */
};

const struct pad_config *variant_sleep_gpio_table(u8 slp_typ, size_t *num)
{
	if (slp_typ == ACPI_S5) {
		*num = ARRAY_SIZE(s5_sleep_gpio_table);
		return s5_sleep_gpio_table;
	}
	*num = 0;
	return NULL;
}
