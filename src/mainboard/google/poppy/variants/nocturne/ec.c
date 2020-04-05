/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpi.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <ec/google/chromeec/ec.h>

#include <variant/ec.h>

const struct google_chromeec_event_info *variant_get_event_info(void)
{
	static struct google_chromeec_event_info info = {
		.log_events = MAINBOARD_EC_LOG_EVENTS,
		.sci_events = MAINBOARD_EC_SCI_EVENTS,
		.smi_events = MAINBOARD_EC_SMI_EVENTS,
		.s3_wake_events = MAINBOARD_EC_S3_WAKE_EVENTS,
		.s5_wake_events = MAINBOARD_EC_S5_WAKE_EVENTS,
		.s0ix_wake_events = MAINBOARD_EC_S0IX_WAKE_EVENTS,
	};

	if (board_id() < 2)
		info.sci_events |= EC_HOST_EVENT_MASK(EC_HOST_EVENT_MKBP);

	return &info;
}
