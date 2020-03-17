/*
 * This file is part of the coreboot project.
 *
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
#include <console/console.h>
#include <cpu/x86/smm.h>

#include "commands.h"
#include "ec.h"
#include "smm.h"

void wilco_ec_smi_sleep(int slp_type)
{
	switch (slp_type) {
	case ACPI_S3:
		wilco_ec_send_noargs(KB_SAVE);
		wilco_ec_change_wake((EC_ACPI_WAKE_PWRB|EC_ACPI_WAKE_LID),
				WAKE_ON);
		wilco_ec_slp_en();
		break;
	case ACPI_S5:
		wilco_ec_power_off(EC_SWOFF_ACPI);
		break;
	}
}

void wilco_ec_smi_apmc(int apmc)
{
	switch (apmc) {
	case APM_CNT_ACPI_ENABLE:
		wilco_ec_send(KB_ACPI, ACPI_ON);
		break;
	case APM_CNT_ACPI_DISABLE:
		wilco_ec_send(KB_ACPI, ACPI_OFF);
		break;
	}
}

void wilco_ec_smi_espi(void)
{
	struct ec_pm_event_state pm;

	if (!wilco_ec_get_pm(&pm, true)) {
		printk(BIOS_INFO,
		       "EC SMI: %02x_%02x-%02x_%02x_%02x_%02x_%02x_%02x-%02x",
		       pm.event[0], pm.event[1],
		       pm.state[0], pm.state[1], pm.state[2],
		       pm.state[3], pm.state[4], pm.state[5], pm.hotkey);
	}
}
