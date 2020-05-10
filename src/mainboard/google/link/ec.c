/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <types.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include "ec.h"

void link_ec_init(void)
{
	const struct google_chromeec_event_info info = {
		.log_events = LINK_EC_LOG_EVENTS,
		.sci_events = LINK_EC_SCI_EVENTS,
		.s3_wake_events = LINK_EC_S3_WAKE_EVENTS,
		.s5_wake_events = LINK_EC_S5_WAKE_EVENTS,
	};

	printk(BIOS_DEBUG, "link_ec_init\n");
	post_code(0xf0);

	google_chromeec_events_init(&info, acpi_is_wakeup_s3());

	post_code(0xf1);
}
