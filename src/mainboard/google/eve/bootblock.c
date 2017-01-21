/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 * Copyright (C) 2016 Intel Corporation
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

#include <arch/io.h>
#include <bootblock_common.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/pm.h>
#include "gpio.h"

static void early_config_gpio(void)
{
	gpio_configure_pads(early_gpio_table, ARRAY_SIZE(early_gpio_table));
}

void bootblock_mainboard_init(void)
{
	uint32_t pm1_cnt = inl(ACPI_BASE_ADDRESS + PM1_CNT);
	uint32_t pm1_sts = inl(ACPI_BASE_ADDRESS + PM1_STS);

	/* Turn on keyboard backlight to indicate we are booting */
	if ((pm1_sts & WAK_STS) && (acpi_sleep_from_pm1(pm1_cnt) != ACPI_S3))
		google_chromeec_kbbacklight(75);

	early_config_gpio();
}
