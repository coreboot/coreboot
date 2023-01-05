/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/nvs.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <northbridge/intel/sandybridge/sandybridge.h>

/* Include EC functions */
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/smm.h>
#include "ec.h"

void mainboard_smi_gpi(u32 gpi_sts)
{
	if (gpi_sts & (1 << EC_SMI_GPI))
		chromeec_smi_process_events();
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
	while (google_chromeec_get_event() != EC_HOST_EVENT_NONE)
		;

	/* Enable wake events */
	google_chromeec_set_wake_mask(MAINBOARD_EC_S3_WAKE_EVENTS);
}

int mainboard_smi_apmc(u8 apmc)
{
	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS, MAINBOARD_EC_SMI_EVENTS);
	return 0;
}
