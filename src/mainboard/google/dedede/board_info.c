/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <baseboard/variants.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <smbios.h>
#include <string.h>

int board_info_get_fw_config(uint32_t *fw_config)
{
	return google_chromeec_cbi_get_fw_config(fw_config);
}
