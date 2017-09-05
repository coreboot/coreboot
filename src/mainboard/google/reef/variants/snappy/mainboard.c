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

#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

void variant_board_ec_set_skuid(void)
{
	google_chromeec_set_sku_id(variant_board_sku());
}
