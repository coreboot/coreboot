/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include "pin_mux.h"

void mainboard_silicon_init_params(FSP_S_CONFIG *supd)
{
	configure_pin_mux(supd);
	supd->TcNotifyIgd = 2; // Auto
}
