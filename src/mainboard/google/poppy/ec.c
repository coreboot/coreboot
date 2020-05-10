/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>

#include <variant/ec.h>

__weak const struct google_chromeec_event_info *variant_get_event_info(void)
{
	static const struct google_chromeec_event_info info = {
		.log_events = MAINBOARD_EC_LOG_EVENTS,
		.sci_events = MAINBOARD_EC_SCI_EVENTS,
		.smi_events = MAINBOARD_EC_SMI_EVENTS,
		.s3_wake_events = MAINBOARD_EC_S3_WAKE_EVENTS,
		.s5_wake_events = MAINBOARD_EC_S5_WAKE_EVENTS,
		.s0ix_wake_events = MAINBOARD_EC_S0IX_WAKE_EVENTS,
	};

	return &info;
}

void mainboard_ec_init(void)
{
	google_chromeec_events_init(variant_get_event_info(),
				    acpi_is_wakeup_s3());
}
