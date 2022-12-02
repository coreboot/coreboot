/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <soc/pm.h>
#include <ec/google/chromeec/ec.h>
#include <southbridge/intel/lynxpoint/lp_gpio.h>
#include <soc/iomap.h>
#include "onboard.h"

/* gpi_sts is GPIO 47:32 */
void mainboard_smi_gpi(u32 gpi_sts)
{
}

void mainboard_smi_sleep(u8 slp_typ)
{
	/* Disable USB charging if required */
	switch (slp_typ) {
	case ACPI_S3:
		set_power_led(LED_BLINK);

		/* Enable DCP mode */
		if (CONFIG(BOARD_GOOGLE_TIDUS)) {
			set_gpio(GPIO_USB_CTL_1, 0);
		}
		break;
	case ACPI_S5:
		set_power_led(LED_OFF);
		break;
	}
}

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APM_CNT_ACPI_ENABLE:
		break;
	case APM_CNT_ACPI_DISABLE:
		break;
	}
	return 0;
}
