/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <drivers/intel/gma/opregion.h>
#include <soc/ramstage.h>
#include <variants.h>
#include <common/pin_mux.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *supd)
{
	configure_pin_mux(supd);
	supd->TcNotifyIgd = 2; // Auto
}

const char *mainboard_vbt_filename(void)
{
	if (get_uint_option("display_native_res", 0) == 1)
		return "vbt_native_res.bin";


	if (get_memory_config_straps() == 13)
		return "vbt_qhd.bin";
	return "vbt.bin";
}
