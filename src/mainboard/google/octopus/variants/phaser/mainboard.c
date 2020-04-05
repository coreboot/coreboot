/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include <sar.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;
	uint32_t sku_id;

	if (google_chromeec_cbi_get_sku_id(&sku_id))
		return NULL;

	if (sku_id == 5)
		filename = "wifi_sar-laser.hex";

	return filename;
}
