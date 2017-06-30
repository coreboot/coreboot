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

#include <arch/acpi.h>
#include <ec/google/chromeec/ec.h>
#include "ec.h"

void mainboard_ec_init(void)
{
	if (acpi_is_wakeup_s3()) {
		google_chromeec_log_events(MAINBOARD_EC_LOG_EVENTS |
					   MAINBOARD_EC_S3_WAKE_EVENTS);

		/* Log and clear device events that may wake the system */
		google_chromeec_log_device_events(
			MAINBOARD_EC_S3_DEVICE_EVENTS);

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
