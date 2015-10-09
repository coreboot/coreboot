/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Google Inc.
 * Copyright (C) 2015 Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/acpi.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include "ec.h"

void mainboard_ec_init(void)
{
	printk(BIOS_DEBUG, "mainboard: EC init\n");

	if (acpi_is_wakeup_s3()) {
		google_chromeec_log_events(MAINBOARD_EC_LOG_EVENTS |
					   MAINBOARD_EC_S3_WAKE_EVENTS);

		/* Disable SMI and wake events */
		google_chromeec_set_smi_mask(0);

		/* Clear pending events */
		while (google_chromeec_get_event() != 0)
			;

		/* Restore SCI event mask */
		google_chromeec_set_sci_mask(MAINBOARD_EC_SCI_EVENTS);
	} else {
		google_chromeec_log_events(MAINBOARD_EC_LOG_EVENTS |
					   MAINBOARD_EC_S5_WAKE_EVENTS);
	}

	/* Clear wake event mask */
	google_chromeec_set_wake_mask(0);
}
