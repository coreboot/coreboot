/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <drivers/intel/gma/opregion.h>
#include <fw_config.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;

	if (fw_config_probe(FW_CONFIG(TABLETMODE, TABLETMODE_ENABLED)))
		filename = "wifi_sar-drawcia.hex";

	return filename;
}

const char *mainboard_vbt_filename(void)
{
	if (fw_config_probe(FW_CONFIG(DB_PORTS, DB_PORTS_1A_HDMI)))
		return "vbt_drawman.bin";

	return "vbt.bin";
}
