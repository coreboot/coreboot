/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <common/fsp_params.h>
#include <common/pin_mux.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *supd)
{
	configure_pin_mux(supd);
	starlabs_update_fsp_s_policy(supd);
	supd->TcNotifyIgd = 2; // Auto
}
