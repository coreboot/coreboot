/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Google, LLC.
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

#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

void variant_romstage_entry(int s3_resume)
{
	uint32_t sku = google_chromeec_get_sku_id();

	/* Based on SKU, turn on keyboard backlight to show system is booting */
	if (sku <= 6 && !s3_resume)
		google_chromeec_kbbacklight(75);
}
