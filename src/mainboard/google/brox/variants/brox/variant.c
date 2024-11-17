/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>
#include <sar.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	uint32_t board_version = board_id();

	if (fw_config_probe(FW_CONFIG(WIFI_BT, WIFI_BT_CNVI))) {
		printk(BIOS_INFO, "CNVi bluetooth enabled by fw_config\n");
		config->cnvi_bt_core = true;
	}

	/* Disable I2C bus device for Touchscreen for board version 1*/
	if (board_version >= 1)
		config->serial_io_i2c_mode[PchSerialIoIndexI2C1] = PchSerialIoDisabled;
}

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_BT));
}

const char *variant_get_auxfw_version_file(void)
{
	if (fw_config_probe(FW_CONFIG(RETIMER, RETIMER_BYPASS)))
		return "rts5453_retimer_bypass.hash";
	else if (fw_config_probe(FW_CONFIG(RETIMER, RETIMER_JHL8040)))
		return "rts5453_retimer_jhl8040.hash";
	return NULL;
}
