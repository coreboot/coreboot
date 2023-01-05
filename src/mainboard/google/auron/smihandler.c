/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/pm.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/smm.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include <soc/iomap.h>
#include <soc/nvs.h>
#include "ec.h"
#include <variant/onboard.h>

/* gpi_sts is GPIO 47:32 */
void mainboard_smi_gpi(u32 gpi_sts)
{
	if (gpi_sts & (1 << (EC_SMI_GPI - 32)))
		chromeec_smi_process_events();
}

static void mainboard_disable_gpios(void)
{
#if CONFIG(BOARD_GOOGLE_SAMUS)
	/* Put SSD in reset to prevent leak */
	set_gpio(BOARD_SSD_RESET_GPIO, 0);
	/* Disable LTE */
	set_gpio(BOARD_LTE_DISABLE_GPIO, 0);
#else
	set_gpio(BOARD_PP3300_CODEC_GPIO, 0);
#endif
	/* Prevent leak from standby rail to WLAN rail */
	set_gpio(BOARD_WLAN_DISABLE_GPIO, 0);
}

void mainboard_smi_sleep(u8 slp_typ)
{
	/* Disable USB charging if required */
	switch (slp_typ) {
	case ACPI_S3:
		if (gnvs->s3u0 == 0) {
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
			google_chromeec_set_usb_charge_mode(
				1, USB_CHARGE_MODE_DISABLED);
		}

		mainboard_disable_gpios();

		/* Enable wake events */
		google_chromeec_set_wake_mask(MAINBOARD_EC_S3_WAKE_EVENTS);
		break;
	case ACPI_S5:
		if (gnvs->s5u0 == 0) {
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
			google_chromeec_set_usb_charge_mode(
				1, USB_CHARGE_MODE_DISABLED);
		}

		mainboard_disable_gpios();

		/* Enable wake events */
		google_chromeec_set_wake_mask(MAINBOARD_EC_S5_WAKE_EVENTS);
		break;
	}

	/* Disable SCI and SMI events */
	google_chromeec_set_smi_mask(0);
	google_chromeec_set_sci_mask(0);

	/* Clear pending events that may trigger immediate wake */
	while (google_chromeec_get_event() != EC_HOST_EVENT_NONE)
		;
}

int mainboard_smi_apmc(u8 apmc)
{
	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS, MAINBOARD_EC_SMI_EVENTS);
	return 0;
}
