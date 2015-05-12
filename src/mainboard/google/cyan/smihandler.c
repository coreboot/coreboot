/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright (C) 2015 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include "ec.h"
#include <ec/google/chromeec/ec.h>
#include <elog.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <soc/gpio.h>

#include "onboard.h"

/* The wake gpio is SUS_GPIO[0]. */
#define WAKE_GPIO_EN SUS_GPIO_EN0

int mainboard_io_trap_handler(int smif)
{
	switch (smif) {
	case 0x99:
		printk(BIOS_DEBUG, "Sample\n");
		smm_get_gnvs()->smif = 0;
		break;
	default:
		return 0;
	}

	/*
	 * On success, the IO Trap Handler returns 0
	 * On failure, the IO Trap Handler returns a value != 0
	 *
	 * For now, we force the return value to 0 and log all traps to
	 * see what's going on.
	 */
	//gnvs->smif = 0;
	return 1;
}

#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
static uint8_t mainboard_smi_ec(void)
{
	uint8_t cmd = google_chromeec_get_event();
	uint16_t pmbase = get_pmbase();
	uint32_t pm1_cnt;

#if IS_ENABLED(CONFIG_ELOG_GSMI)
	/* Log this event */
	if (cmd)
		elog_add_event_byte(ELOG_TYPE_EC_EVENT, cmd);
#endif

	switch (cmd) {
	case EC_HOST_EVENT_LID_CLOSED:
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

		/* Go to S5 */
		pm1_cnt = inl(pmbase + PM1_CNT);
		pm1_cnt |= SLP_EN | (SLP_TYP_S5 << SLP_TYP_SHIFT);
		outl(pm1_cnt, pmbase + PM1_CNT);
		break;
	}

	return cmd;
}
#endif

/*
 * The entire 32-bit ALT_GPIO_SMI register is passed as a parameter. Note, that
 * this includes the enable bits in the lower 16 bits.
 */
void mainboard_smi_gpi(uint32_t alt_gpio_smi)
{
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
	if (alt_gpio_smi & (1 << EC_SMI_GPI)) {
		/* Process all pending events */
		while (mainboard_smi_ec() != 0)
			;
	}
#endif
}

void mainboard_smi_sleep(uint8_t slp_typ)
{
	/* Disable USB charging if required */
	switch (slp_typ) {
	case 3:
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
		if (smm_get_gnvs()->s3u0 == 0)
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
		if (smm_get_gnvs()->s3u1 == 0)
			google_chromeec_set_usb_charge_mode(
				1, USB_CHARGE_MODE_DISABLED);

		/* Enable wake events */
		google_chromeec_set_wake_mask(MAINBOARD_EC_S3_WAKE_EVENTS);
#endif
		/* Enable wake pin in GPE block. */
		enable_gpe(WAKE_GPIO_EN);
		break;
	case 5:
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
		if (smm_get_gnvs()->s5u0 == 0)
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
		if (smm_get_gnvs()->s5u1 == 0)
			google_chromeec_set_usb_charge_mode(
				1, USB_CHARGE_MODE_DISABLED);

		/* Enable wake events */
		google_chromeec_set_wake_mask(MAINBOARD_EC_S5_WAKE_EVENTS);
#endif
		break;
	}

#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
	/* Disable SCI and SMI events */
	google_chromeec_set_smi_mask(0);
	google_chromeec_set_sci_mask(0);

	/* Clear pending events that may trigger immediate wake */
	while (google_chromeec_get_event() != 0)
		;

        if (smm_get_gnvs()->bdid == BOARD_PRE_EVT) {
                /* Set LPC lines to low power in S3/S5. */
                if ((slp_typ == SLEEP_STATE_S3) || (slp_typ == SLEEP_STATE_S5))
                        lpc_set_low_power();
        }

#endif
}

int mainboard_smi_apmc(uint8_t apmc)
{
	switch (apmc) {
	case APM_CNT_ACPI_ENABLE:
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
		google_chromeec_set_smi_mask(0);
		/* Clear all pending events */
		while (google_chromeec_get_event() != 0)
			;
		google_chromeec_set_sci_mask(MAINBOARD_EC_SCI_EVENTS);
#endif
		break;
	case APM_CNT_ACPI_DISABLE:
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
		google_chromeec_set_sci_mask(0);
		/* Clear all pending events */
		while (google_chromeec_get_event() != 0)
			;
		google_chromeec_set_smi_mask(MAINBOARD_EC_SMI_EVENTS);
#endif
		break;
	}
	return 0;
}
