/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpi.h>
#include <baseboard/variants.h>
#include <gpio.h>

#define TS_ENABLE	GPP_B4

void variant_smi_sleep(u8 slp_typ)
{
	if (slp_typ == ACPI_S5) {
		/* Set TS to disable */
		gpio_set(TS_ENABLE, 0);
	}
}
