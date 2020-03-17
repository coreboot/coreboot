/*
 * This file is part of the coreboot project.
 *
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

#include <ec/google/chromeec/ec.h>
#include <baseboard/variants.h>
#include <sar.h>

#define RIGHT_USB_C_PORT_ID 4

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id == 2)
		filename = "wifi_sar-bluebird.hex";

	return filename;
}

bool variant_ext_usb_status(unsigned int port_type, unsigned int port_id)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id == 2 && port_id == RIGHT_USB_C_PORT_ID)
		return false;
	return true;
}
