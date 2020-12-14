/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include "variant.h"

void mainboard_silicon_init_params(FSPS_UPD *supd)
{
	/* Configure pads prior to SiliconInit() in case there's any
	 * dependencies during hardware initialization. */
	size_t num_gpios;
	const struct pad_config *gpio_table = variant_gpio_table(&num_gpios);
	cnl_configure_pads(gpio_table, num_gpios);
}
