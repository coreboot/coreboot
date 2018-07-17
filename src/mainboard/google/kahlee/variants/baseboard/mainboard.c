/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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
#include <gpio.h>
#include <variant/gpio.h>


uint8_t variant_board_sku(void)
{
	static int sku = -1;

	if (sku == -1)
		sku = google_chromeec_get_sku_id();

	return sku;
}

#if IS_ENABLED(CONFIG_HAVE_ACPI_RESUME)
void variant_mainboard_suspend_resume(void)
{
	/* Enable backlight - GPIO 133 active low */
	gpio_set(GPIO_133, 0);
}
#endif
