/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <variant/gpio.h>
#include "variants/baseboard/include/eeprom.h"

void mainboard_silicon_init_params(FSPS_UPD *supd)
{
	FSP_S_CONFIG *params = &supd->FspsConfig;

	/* Configure pads prior to SiliconInit() in case there's any
	   dependencies during hardware initialization. */
	program_gpio_pads();

	params->SataLedEnable = 1;

	/* Overwrite params */
	if (!check_signature(offsetof(struct eeprom_layout, supd), FSPS_UPD_SIGNATURE))
		return;

	READ_EEPROM_FSP_S(supd, FspsTestConfig.VtdDisableDeprecated);
	READ_EEPROM_FSP_S(supd, FspsConfig.PchPmWolEnableOverride);
}
