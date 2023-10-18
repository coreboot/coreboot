/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <fw_config.h>
#include <baseboard/variants.h>

void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config)
{
	if (fw_config_probe(FW_CONFIG(FP_MCU, FP_ABSENT)))
		config->serial_io_gspi_mode[PchSerialIoIndexGSPI1] = PchSerialIoDisabled;
}
