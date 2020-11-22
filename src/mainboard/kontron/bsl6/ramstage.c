/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <mainboard/gpio.h>

void mainboard_silicon_init_params(FSP_SIL_UPD *params)
{
	mainboard_configure_gpios();
}
