/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <soc/ramstage.h>
#include <common/fsp_params.h>
#include <common/pin_mux.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *supd)
{
	configure_pin_mux(supd);
	starlabs_update_fsp_s_policy(supd);

	if (get_uint_option("thunderbolt", 1) == 0)
		supd->UsbTcPortEn = 0;
}
