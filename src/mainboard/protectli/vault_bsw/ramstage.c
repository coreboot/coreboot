/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/ramstage.h>

void mainboard_silicon_init_params(SILICON_INIT_UPD *params)
{
	params->PcdTurboMode = 1;
}
