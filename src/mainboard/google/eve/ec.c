/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpi.h>
#include <ec/google/chromeec/ec.h>
#include "ec.h"

void mainboard_ec_init(void)
{
	const struct google_chromeec_event_info info = {
		.log_events = MAINBOARD_EC_LOG_EVENTS,
		.sci_events = MAINBOARD_EC_SCI_EVENTS,
		.s3_wake_events = MAINBOARD_EC_S3_WAKE_EVENTS,
		.s3_device_events = MAINBOARD_EC_S3_DEVICE_EVENTS,
		.s5_wake_events = MAINBOARD_EC_S5_WAKE_EVENTS,
	};

	google_chromeec_events_init(&info, acpi_is_wakeup_s3());
}
