/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

int board_info_get_fw_config(uint64_t *fw_config)
{
	return google_chromeec_cbi_get_fw_config(fw_config);
}
