/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cpu/intel/haswell/haswell.h>
#include <cpu/x86/smm.h>
#include <gpio.h>
#include <northbridge/intel/haswell/haswell.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/lynxpoint/me.h>

/* Include EC functions */
#include <ec/google/chromeec/smm.h>
#include "ec.h"

/* Codec enable: GPIO45 */
#define GPIO_PP3300_CODEC_EN 45
/* GPIO46 controls the WLAN_DISABLE_L signal. */
#define GPIO_WLAN_DISABLE_L 46
#define GPIO_LTE_DISABLE_L  59

/* gpi_sts is GPIO 47:32 */
void mainboard_smi_gpi(u32 gpi_sts)
{
	if (gpi_sts & (1 << (EC_SMI_GPI - 32)))
		chromeec_smi_process_events();
}

void mainboard_smi_sleep(u8 slp_typ)
{
	/* Disable USB charging if required */
	chromeec_set_usb_charge_mode(slp_typ);

	switch (slp_typ) {
	case ACPI_S3:
	case ACPI_S4:
	case ACPI_S5:
		/* Prevent leak from standby rail to WLAN rail in S3/S4/S5. */
		gpio_set(GPIO_WLAN_DISABLE_L, 0);
		gpio_set(GPIO_PP3300_CODEC_EN, 0);
		/* Disable LTE */
		gpio_set(GPIO_LTE_DISABLE_L, 0);
		break;
	}

	chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS, MAINBOARD_EC_S5_WAKE_EVENTS);
}

int mainboard_smi_apmc(u8 apmc)
{
	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS, MAINBOARD_EC_SMI_EVENTS);
	return 0;
}
