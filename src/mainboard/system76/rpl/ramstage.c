/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/gpio.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	// TODO: Pin Mux settings

	// Enable reporting CPU C10 state over eSPI.
	params->PchEspiHostC10ReportEnable = 1;
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
};
