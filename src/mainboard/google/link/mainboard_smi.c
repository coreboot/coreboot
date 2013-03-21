/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/bd82x6x/nvs.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <cpu/intel/model_206ax/model_206ax.h>
#include <elog.h>

/* Include romstage serial for SIO helper functions */
#include <superio/ite/it8772f/early_serial.c>

/* Include EC functions */
#include <ec/google/chromeec/ec.h>
#include "ec.h"

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

	/* On success, the IO Trap Handler returns 0
	 * On failure, the IO Trap Handler returns a value != 0
	 *
	 * For now, we force the return value to 0 and log all traps to
	 * see what's going on.
	 */
	//gnvs->smif = 0;
	return 1;
}

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
		pm1_cnt = inl(smm_get_pmbase() + PM1_CNT);
		pm1_cnt |= (0xf << 10);
		outl(pm1_cnt, smm_get_pmbase() + PM1_CNT);
		break;
	}

	return cmd;
}

void mainboard_smi_gpi(u16 gpi_sts)
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
	case 3:
		if (smm_get_gnvs()->s3u0 == 0)
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
		if (smm_get_gnvs()->s3u1 == 0)
			google_chromeec_set_usb_charge_mode(
				1, USB_CHARGE_MODE_DISABLED);
		break;
	case 5:
		if (smm_get_gnvs()->s5u0 == 0)
			google_chromeec_set_usb_charge_mode(
				0, USB_CHARGE_MODE_DISABLED);
		if (smm_get_gnvs()->s5u1 == 0)
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

#define APMC_FINALIZE 0xcb
#define APMC_ACPI_EN  0xe1
#define APMC_ACPI_DIS 0x1e

static int mainboard_finalized = 0;

int mainboard_smi_apmc(u8 apmc)
{
	switch (apmc) {
	case APMC_FINALIZE:
		if (mainboard_finalized) {
			printk(BIOS_DEBUG, "SMI#: Already finalized\n");
			return 0;
		}

		intel_me_finalize_smm();
		intel_pch_finalize_smm();
		intel_sandybridge_finalize_smm();
		intel_model_206ax_finalize_smm();

		mainboard_finalized = 1;
		break;
	case APMC_ACPI_EN:
		google_chromeec_set_smi_mask(0);
		/* Clear all pending events */
		while (google_chromeec_get_event() != 0);
		google_chromeec_set_sci_mask(LINK_EC_SCI_EVENTS);
		break;
	case APMC_ACPI_DIS:
		google_chromeec_set_sci_mask(0);
		/* Clear all pending events */
		while (google_chromeec_get_event() != 0);
		google_chromeec_set_smi_mask(LINK_EC_SMI_EVENTS);;
		break;
	}
	return 0;
}
