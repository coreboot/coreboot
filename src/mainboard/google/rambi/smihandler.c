/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cpu/x86/smm.h>

#include <ec/google/chromeec/smm.h>
#include "ec.h"

#include <soc/pm.h>

/* The wake gpio is SUS_GPIO[0]. */
#define WAKE_GPIO_EN SUS_GPIO_EN0

/* The entire 32-bit ALT_GPIO_SMI register is passed as a parameter. Note, that
 * this includes the enable bits in the lower 16 bits. */
void mainboard_smi_gpi(uint32_t alt_gpio_smi)
{
	if (alt_gpio_smi & (1 << EC_SMI_GPI))
		chromeec_smi_process_events();
}

void mainboard_smi_sleep(uint8_t slp_typ)
{
	/* Disable USB charging if required */
	chromeec_set_usb_charge_mode(slp_typ);

	switch (slp_typ) {
	case ACPI_S3:
		/* Enable wake pin in GPE block. */
		enable_gpe(WAKE_GPIO_EN);
		break;
	}

	chromeec_smi_sleep(slp_typ, MAINBOARD_EC_S3_WAKE_EVENTS, MAINBOARD_EC_S5_WAKE_EVENTS);
}

int mainboard_smi_apmc(uint8_t apmc)
{
	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS, MAINBOARD_EC_SMI_EVENTS);
	return 0;
}
