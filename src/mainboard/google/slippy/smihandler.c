/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 * Copyright 2012 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpi.h>
#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/lynxpoint/nvs.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <southbridge/intel/lynxpoint/me.h>
#include <northbridge/intel/haswell/haswell.h>
#include <cpu/intel/haswell/haswell.h>
#include <elog.h>

/* Include EC functions */
#include <ec/google/chromeec/ec.h>
#include "ec.h"

/* Codec enable: GPIO45 */
#define GPIO_PP3300_CODEC_EN 45
/* GPIO46 controls the WLAN_DISABLE_L signal. */
#define GPIO_WLAN_DISABLE_L 46
#define GPIO_LTE_DISABLE_L  59

static u8 mainboard_smi_ec(void)
{
	u8 cmd = google_chromeec_get_event();
	u32 pm1_cnt;

#if CONFIG_ELOG_GSMI
	/* Log this event */
	if (cmd)
		elog_add_event_byte(ELOG_TYPE_EC_EVENT, cmd);
#endif

	switch (cmd) {
	case EC_HOST_EVENT_LID_CLOSED:
		printk(BIOS_DEBUG, "LID CLOSED, SHUTDOWN\n");

		/* Go to S5 */
		pm1_cnt = inl(get_pmbase() + PM1_CNT);
		pm1_cnt |= (0xf << 10);
		outl(pm1_cnt, get_pmbase() + PM1_CNT);
		break;
	}

	return cmd;
}

/* gpi_sts is GPIO 47:32 */
void mainboard_smi_gpi(u32 gpi_sts)
{
	if (gpi_sts & (1 << (EC_SMI_GPI - 32))) {
		/* Process all pending events */
		while (mainboard_smi_ec() != 0);
	}
}

void mainboard_smi_sleep(u8 slp_typ)
{
	/* Disable USB charging if required */
	switch (slp_typ) {
	case ACPI_S3:
		if (smm_get_gnvs()->s3u0 == 0)
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
		if (smm_get_gnvs()->s3u1 == 0)
			google_chromeec_set_usb_charge_mode(
				1, USB_CHARGE_MODE_DISABLED);

		/* Prevent leak from standby rail to WLAN rail in S3. */
		set_gpio(GPIO_WLAN_DISABLE_L, 0);
		set_gpio(GPIO_PP3300_CODEC_EN, 0);
		/* Disable LTE */
		set_gpio(GPIO_LTE_DISABLE_L, 0);

		/* Enable wake events */
		google_chromeec_set_wake_mask(MAINBOARD_EC_S3_WAKE_EVENTS);
		break;
	case ACPI_S4:
	case ACPI_S5:
		if (smm_get_gnvs()->s5u0 == 0)
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
		if (smm_get_gnvs()->s5u1 == 0)
			google_chromeec_set_usb_charge_mode(
				1, USB_CHARGE_MODE_DISABLED);

		/* Prevent leak from standby rail to WLAN rail in S5. */
		set_gpio(GPIO_WLAN_DISABLE_L, 0);
		set_gpio(GPIO_PP3300_CODEC_EN, 0);
		/* Disable LTE */
		set_gpio(GPIO_LTE_DISABLE_L, 0);

		/* Enable wake events */
		google_chromeec_set_wake_mask(MAINBOARD_EC_S5_WAKE_EVENTS);
		break;
	}

	/* Disable SCI and SMI events */
	google_chromeec_set_smi_mask(0);
	google_chromeec_set_sci_mask(0);

	/* Clear pending events that may trigger immediate wake */
	while (google_chromeec_get_event() != 0);
}


static int mainboard_finalized = 0;

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APM_CNT_FINALIZE:
		if (mainboard_finalized) {
			printk(BIOS_DEBUG, "SMI#: Already finalized\n");
			return 0;
		}

		intel_pch_finalize_smm();
		intel_northbridge_haswell_finalize_smm();
		intel_cpu_haswell_finalize_smm();

		mainboard_finalized = 1;
		break;
	case APM_CNT_ACPI_ENABLE:
		google_chromeec_set_smi_mask(0);
		/* Clear all pending events */
		while (google_chromeec_get_event() != 0);
		google_chromeec_set_sci_mask(MAINBOARD_EC_SCI_EVENTS);
		break;
	case APM_CNT_ACPI_DISABLE:
		google_chromeec_set_sci_mask(0);
		/* Clear all pending events */
		while (google_chromeec_get_event() != 0);
		google_chromeec_set_smi_mask(MAINBOARD_EC_SMI_EVENTS);
		break;
	}
	return 0;
}
