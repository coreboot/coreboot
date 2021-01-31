/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/nvs.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <southbridge/intel/common/pmbase.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <elog.h>

/* Include EC functions */
#include <ec/google/chromeec/ec.h>
#include "ec.h"

static u8 mainboard_smi_ec(void)
{
	u8 cmd = google_chromeec_get_event();

	/* Log this event */
	if (cmd)
		elog_gsmi_add_event_byte(ELOG_TYPE_EC_EVENT, cmd);

	switch (cmd) {
	case EC_HOST_EVENT_LID_CLOSED:
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

		/* Go to S5 */
		write_pmbase32(PM1_CNT, read_pmbase32(PM1_CNT) | (0xf << 10));
		break;
	}

	return cmd;
}

void mainboard_smi_gpi(u32 gpi_sts)
{
	if (gpi_sts & (1 << EC_SMI_GPI)) {
		/* Process all pending events */
		while (mainboard_smi_ec() != 0);
	}
}

void mainboard_smi_sleep(u8 slp_typ)
{
	/* Disable USB charging if required */
	switch (slp_typ) {
	case ACPI_S3:
		if (gnvs->s3u0 == 0)
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
		if (gnvs->s3u1 == 0)
			google_chromeec_set_usb_charge_mode(
				1, USB_CHARGE_MODE_DISABLED);
		break;
	case ACPI_S5:
		if (gnvs->s5u0 == 0)
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
		if (gnvs->s5u1 == 0)
			google_chromeec_set_usb_charge_mode(
				1, USB_CHARGE_MODE_DISABLED);
		break;
	}

	/* Disable SCI and SMI events */
	google_chromeec_set_smi_mask(0);
	google_chromeec_set_sci_mask(0);

	/* Clear pending events that may trigger immediate wake */
	while (google_chromeec_get_event() != 0);

	/* Enable wake events */
	google_chromeec_set_wake_mask(LINK_EC_S3_WAKE_EVENTS);
}

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APM_CNT_ACPI_ENABLE:
		google_chromeec_set_smi_mask(0);
		/* Clear all pending events */
		while (google_chromeec_get_event() != 0);
		google_chromeec_set_sci_mask(LINK_EC_SCI_EVENTS);
		break;
	case APM_CNT_ACPI_DISABLE:
		google_chromeec_set_sci_mask(0);
		/* Clear all pending events */
		while (google_chromeec_get_event() != 0);
		google_chromeec_set_smi_mask(LINK_EC_SMI_EVENTS);
		break;
	}
	return 0;
}
