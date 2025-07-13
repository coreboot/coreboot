/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cpu/x86/smm.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/smm.h>
#include <gpio.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include <soc/iomap.h>
#include <soc/pm.h>
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
	gpio_set(BOARD_SSD_RESET_GPIO, 0);
	/* Disable LTE */
	gpio_set(BOARD_LTE_DISABLE_GPIO, 0);
#else
	gpio_set(BOARD_PP3300_CODEC_GPIO, 0);
#endif
	/* Prevent leak from standby rail to WLAN rail */
	gpio_set(BOARD_WLAN_DISABLE_GPIO, 0);
}

void mainboard_smi_sleep(u8 slp_typ)
{
	/* Disable USB charging if required */
	/* NOTE: Setting of usb0 _may_ also control usb1 here. */
	chromeec_set_usb_charge_mode(slp_typ);

	switch (slp_typ) {
	case ACPI_S3:
	case ACPI_S5:
		mainboard_disable_gpios();
		break;
	}

	chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS, MAINBOARD_EC_S5_WAKE_EVENTS);
}

int mainboard_smi_apmc(u8 apmc)
{
	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS, MAINBOARD_EC_SMI_EVENTS);
	return 0;
}
