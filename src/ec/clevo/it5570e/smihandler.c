/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <cpu/x86/smm.h>

#include "commands.h"
#include "smm.h"

void ec_smi_apmc(uint8_t apmc)
{
	printk(BIOS_DEBUG, "EC SMI APMC handler\n");

	switch (apmc) {
	case APM_CNT_ACPI_ENABLE:
		ec_set_acpi_mode(true);
		break;
	case APM_CNT_ACPI_DISABLE:
		ec_set_acpi_mode(false);
		break;
	default:
		break;
	}
}

void ec_smi_sleep(uint8_t slp_type)
{
	printk(BIOS_DEBUG, "EC SMI sleep handler\n");

	switch (slp_type) {
	case ACPI_S4:
	case ACPI_S5:
		ec_set_enter_g3_in_s4s5(true);
		__fallthrough;
	default:
		break;
	}
}
