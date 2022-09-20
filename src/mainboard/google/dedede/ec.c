/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <console/console.h>
#include <ec/ec.h>
#include <ec/google/chromeec/ec.h>
#include <intelblocks/lpc_lib.h>
#include <variant/ec.h>

void mainboard_ec_init(void)
{
	static const struct google_chromeec_event_info info = {
		.log_events = MAINBOARD_EC_LOG_EVENTS,
		.sci_events = MAINBOARD_EC_SCI_EVENTS,
		.s3_wake_events = MAINBOARD_EC_S3_WAKE_EVENTS,
		.s5_wake_events = MAINBOARD_EC_S5_WAKE_EVENTS,
		.s0ix_wake_events = MAINBOARD_EC_S0IX_WAKE_EVENTS,
	};

	printk(BIOS_INFO, "mainboard: EC init\n");

	google_chromeec_events_init(&info, acpi_is_wakeup_s3());
}
