/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/intel/gma/opregion.h>
#include <soc/ramstage.h>
#include <option.h>
#include <common/pin_mux.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *supd)
{
	configure_pin_mux(supd);
}

const char *mainboard_vbt_filename(void)
{
	if (get_uint_option("display_native_res", 0) == 1)
		return "vbt_native_res.bin";

	return "vbt.bin";
}
