/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/mmio.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include "ec.h"
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/smm.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <soc/gpio.h>

#include <variant/onboard.h>

/* The wake gpio is SUS_GPIO[0]. */
#define WAKE_GPIO_EN SUS_GPIO_EN0
#define GPIO_SUS7_WAKE_MASK     (1 << 12)
#define GPIO_SUS1_WAKE_MASK     (1 << 13)

/*
 * The entire 32-bit ALT_GPIO_SMI register is passed as a parameter. Note, that
 * this includes the enable bits in the lower 16 bits.
 */
void mainboard_smi_gpi(uint32_t alt_gpio_smi)
{
	if (alt_gpio_smi & (1 << EC_SMI_GPI))
		chromeec_smi_process_events();
}

void mainboard_smi_sleep(uint8_t slp_typ)
{
	void		*addr;
	uint32_t	mask;

	/* Disable USB charging if required */
	switch (slp_typ) {
	case ACPI_S3:
		if (gnvs->s3u0 == 0)
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
		if (gnvs->s3u1 == 0)
			google_chromeec_set_usb_charge_mode(
				1, USB_CHARGE_MODE_DISABLED);

		/* Enable wake events */
		google_chromeec_set_wake_mask(MAINBOARD_EC_S3_WAKE_EVENTS);
		/* Enable wake pin in GPE block. */
		enable_gpe(WAKE_GPIO_EN);
		break;
	case ACPI_S5:
		if (gnvs->s5u0 == 0)
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
		if (gnvs->s5u1 == 0)
			google_chromeec_set_usb_charge_mode(
				1, USB_CHARGE_MODE_DISABLED);

		/* Enable wake events */
		google_chromeec_set_wake_mask(MAINBOARD_EC_S5_WAKE_EVENTS);

		/* Disabling wake from SUS_GPIO1 (TOUCH INT) and
		 * SUS_GPIO7 (TRACKPAD INT) in North bank as they are not
		 * valid S5 wake sources
		 */
		addr = (void *)(IO_BASE_ADDRESS + COMMUNITY_OFFSET_GPNORTH +
			GPIO_WAKE_MASK_REG0);
		mask = ~(GPIO_SUS1_WAKE_MASK | GPIO_SUS7_WAKE_MASK);
		write32(addr, read32(addr) & mask);

		break;
	}

	/* Disable SCI and SMI events */
	google_chromeec_set_smi_mask(0);
	google_chromeec_set_sci_mask(0);

	/* Clear pending events that may trigger immediate wake */
	while (google_chromeec_get_event() != EC_HOST_EVENT_NONE)
		;

	/* Set LPC lines to low power in S3/S5. */
	if ((slp_typ == ACPI_S3) || (slp_typ == ACPI_S5)) {
		lpc_set_low_power();
	}
}

int mainboard_smi_apmc(uint8_t apmc)
{
	chromeec_smi_apmc(apmc, MAINBOARD_EC_SCI_EVENTS, MAINBOARD_EC_SMI_EVENTS);
	return 0;
}
