/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <chip.h>
#include <fw_config.h>
#include <sar.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(FP_MCU, FP_ABSENT)))
		config->serial_io_gspi_mode[PchSerialIoIndexGSPI1] = PchSerialIoDisabled;
}

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}
