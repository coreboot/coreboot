/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <ec/google/chromeec/ec.h>
#include <baseboard/variants.h>
#include <delay.h>
#include <gpio.h>

void variant_smi_sleep(u8 slp_typ)
{
	/* Currently use cases here all target to S5 therefore we do early return
	 * here for saving one transaction to the EC for getting SKU ID. */
	if (slp_typ != ACPI_S5)
		return;

	power_off_lte_module();
}
