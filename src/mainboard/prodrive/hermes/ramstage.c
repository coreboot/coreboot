/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <variant/gpio.h>
#include "variants/baseboard/include/eeprom.h"

static fsp_params parmas_list[] = {
	/* FIXME: Fill with additional options */
};

void mainboard_silicon_init_params(FSPS_UPD *supd)
{
	FSP_S_CONFIG *params = &supd->FspsConfig;

	/* Configure pads prior to SiliconInit() in case there's any
	   dependencies during hardware initialization. */
	program_gpio_pads();

	params->SataLedEnable = 1;

	/* Overwrite params */
	if (!check_signature(EEPROM_OFFSET_FSP_SIGNATURE, FSP_UPD_SIGNATURE))
		return;

	for (u8 i = 0; i <= ARRAY_SIZE(parmas_list); i++) {
		if (ARRAY_SIZE(parmas_list) == 0)
			break;
		read_write_config(params, EEPROM_OFFSET_FSP_CONFIG +
			parmas_list[i].offset,
			EEPROM_OFFSET_FSP_CONFIG + parmas_list[i].offset,
			parmas_list[i].size);
	}
}
