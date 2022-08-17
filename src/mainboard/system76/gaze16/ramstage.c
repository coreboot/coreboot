/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <variant/gpio.h>
#include "variant.h"

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	variant_silicon_init_params(params);

	params->PchLegacyIoLowLatency = 1;

	variant_configure_gpios();
}
