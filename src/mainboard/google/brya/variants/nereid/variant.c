/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <drivers/intel/gma/opregion.h>
#include <fw_config.h>

const char *mainboard_vbt_filename(void)
{
	if (fw_config_probe(FW_CONFIG(DB_USB, DB_1A_HDMI)))
		return "vbt-nereid_hdmi.bin";

	return "vbt.bin";
}
