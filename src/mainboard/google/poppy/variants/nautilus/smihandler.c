/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/acpi.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <variant/sku.h>

#define LTE3_PWROFF_L	GPP_D0

void variant_smi_sleep(u8 slp_typ)
{
	uint32_t sku_id;

	if (slp_typ == ACPI_S5) {
		sku_id = variant_board_sku();

		switch (sku_id) {
		case SKU_1_NAUTILUS_LTE:
			/* Turn off LTE module */
			gpio_set(LTE3_PWROFF_L, 0);
			break;
		default:
			break;
		}
	}
}
