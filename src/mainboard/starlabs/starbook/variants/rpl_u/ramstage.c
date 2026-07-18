/* SPDX-License-Identifier: GPL-2.0-only */

#include <common/fsp_params.h>
#include <common/pin_mux.h>
#include <option.h>
#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *supd)
{
	starlabs_update_fsp_s_policy(supd);
	configure_pin_mux(supd);

	if (get_uint_option("thunderbolt", 1) == 0)
		supd->UsbTcPortEn = 0;
}
