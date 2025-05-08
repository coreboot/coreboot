/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <ec/google/chromeec/ec.h>
#include <elog.h>
#include <halt.h>
#include "smm.h"

static int chromeec_process_one_event(void)
{
	uint8_t event = google_chromeec_get_event();

	/* Log this event */
	if (event)
		elog_gsmi_add_event_byte(ELOG_TYPE_EC_EVENT, event);

	switch (event) {
	case EC_HOST_EVENT_LID_CLOSED:
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

		poweroff();
		break;
	}
	return !!event;
}

void chromeec_smi_process_events(void)
{
	/* Process all pending events */
	while (chromeec_process_one_event())
		;
}

static void clear_pending_events(void)
{
	struct ec_response_get_next_event mkbp_event;

	while (google_chromeec_get_event() != EC_HOST_EVENT_NONE)
		;

	printk(BIOS_DEBUG, "Clearing pending EC events. Error code EC_RES_UNAVAILABLE(9) is expected.\n");
	while (google_chromeec_get_mkbp_event(&mkbp_event) == 0)
		;
}

void chromeec_set_usb_charge_mode(int slp_type)
{
	bool usb0_disable = 0, usb1_disable = 0;

	usb_charge_mode_from_gnvs(slp_type, &usb0_disable, &usb1_disable);

	if (usb0_disable)
		google_chromeec_set_usb_charge_mode(0, USB_CHARGE_MODE_DISABLED);

	if (usb1_disable)
		google_chromeec_set_usb_charge_mode(1, USB_CHARGE_MODE_DISABLED);
}

void chromeec_smi_sleep(int slp_type, uint64_t s3_mask, uint64_t s5_mask)
{
	/* Disable SCI and SMI events */
	google_chromeec_set_smi_mask(0);
	google_chromeec_set_sci_mask(0);

	/* Clear pending events that may trigger immediate wake */
	clear_pending_events();

	if (!google_chromeec_is_uhepi_supported()) {
		switch (slp_type) {
		case ACPI_S3:
			/* Enable wake events */
			google_chromeec_set_wake_mask(s3_mask);
			break;
		case ACPI_S4:
		case ACPI_S5:
			/* Enable wake events */
			google_chromeec_set_wake_mask(s5_mask);
			break;
		}
	}
}

void chromeec_smi_device_event_sleep(int slp_type, uint64_t s3_mask,
				     uint64_t s5_mask)
{
	switch (slp_type) {
	case ACPI_S3:
		/* Enable device wake events */
		google_chromeec_set_device_enabled_events(s3_mask);
		break;
	case ACPI_S5:
		/* Enable device wake events */
		google_chromeec_set_device_enabled_events(s5_mask);
		break;
	}

	/* Read and clear pending events that may trigger immediate wake */
	google_chromeec_get_device_current_events();
}

void chromeec_smi_apmc(int apmc, uint64_t sci_mask, uint64_t smi_mask)
{
	switch (apmc) {
	case APM_CNT_ACPI_ENABLE:
		google_chromeec_set_smi_mask(0);
		clear_pending_events();
		google_chromeec_set_sci_mask(sci_mask);
		break;
	case APM_CNT_ACPI_DISABLE:
		google_chromeec_set_sci_mask(0);
		clear_pending_events();
		google_chromeec_set_smi_mask(smi_mask);
		break;
	}
}
