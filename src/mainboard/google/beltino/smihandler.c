/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cpu/intel/haswell/haswell.h>
#include <cpu/x86/smm.h>
#include <northbridge/intel/haswell/haswell.h>
#include <southbridge/intel/lynxpoint/me.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <superio/ite/it8772f/it8772f.h>

#include "onboard.h"

void mainboard_smi_sleep(u8 slp_typ)
{
	switch (slp_typ) {
	case ACPI_S3:
		set_power_led(LED_BLINK);
		break;
	case ACPI_S4:
	case ACPI_S5:
		set_power_led(LED_OFF);
		break;
	default:
		break;
	}
	return;
}
