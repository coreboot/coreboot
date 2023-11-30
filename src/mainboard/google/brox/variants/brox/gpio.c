/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <soc/gpio.h>

const struct pad_config *variant_gpio_override_table(size_t *num)
{
	*num = 0;
	return NULL;
}
