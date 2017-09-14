/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016, 2017 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <string.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>
#include <vendorcode/google/chromeos/cros_vpd.h>

void variant_board_ec_set_skuid(void)
{
	google_chromeec_set_sku_id(variant_board_sku());
}

/*
 * override SKU ID by VPD definition
 */
uint8_t variant_board_sku(void)
{
	static int board_sku_num = -1;
	int vpd_sku_num;
	char vpd_skuid[] = "skuid";
	char vpd_buffer[4];
	int vpd_len;
	int i;

	if (board_sku_num != -1)
		return board_sku_num;

	board_sku_num = sku_strapping_value();

	if (!IS_ENABLED(CONFIG_CHROMEOS))
		return board_sku_num;

	if (!cros_vpd_gets(vpd_skuid, vpd_buffer, ARRAY_SIZE(vpd_buffer)))
		return board_sku_num;

	vpd_len = strlen(vpd_buffer);

	vpd_sku_num = 0;
	for (i = 0; i < vpd_len; i++) {
		char ch = vpd_buffer[i];
		if ((ch < '0') || (ch > '9')) {
			vpd_sku_num = -1;
			break;
		} else
			vpd_sku_num = vpd_sku_num * 10 + (ch - '0');
	}

	if (vpd_sku_num != -1)
		board_sku_num = vpd_sku_num;

	return board_sku_num;
}
