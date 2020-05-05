/* This file is part of the coreboot project. */
/*
 * Copyright 2020 Google LLC
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
#include <variant/sku.h>

void variant_romstage_entry(int s3_resume)
{
	uint32_t sku = google_chromeec_get_sku_id();

	if (!s3_resume) {
		/* Based on SKU, turn on keyboard backlight */
		switch (sku) {
		default:
			google_chromeec_kbbacklight(75);
			break;
		case SKU_CAREENA_KB_BACKLIGHT18:
		case SKU_CAREENA_KB_BACKLIGHT19:
		case SKU_CAREENA_KB_BACKLIGHT22:
		case SKU_CAREENA_KB_BACKLIGHT23:
			google_chromeec_kbbacklight(10);
			break;
		}
	}
}
