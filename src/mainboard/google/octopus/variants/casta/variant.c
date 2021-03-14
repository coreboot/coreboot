/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>
#include <baseboard/variants.h>
#include <sar.h>

#define RIGHT_USB_C_PORT_ID 4

const char *get_wifi_sar_cbfs_filename(void)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id == 2)
		return "wifi_sar-bluebird.hex";

	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}

bool variant_ext_usb_status(unsigned int port_type, unsigned int port_id)
{
	uint32_t sku_id = google_chromeec_get_board_sku();

	if (sku_id == 2 && port_id == RIGHT_USB_C_PORT_ID)
		return false;
	return true;
}
