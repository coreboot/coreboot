/* This file is part of the coreboot project. */
/* SPDX-License-Identifier: GPL-2.0-only */

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
