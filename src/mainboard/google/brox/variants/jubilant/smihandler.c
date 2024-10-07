/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <delay.h>

#include "gpio.h"

#define WWAN_FCPO_L	GPP_F21
#define WWAN_RSL_L	GPP_H23
#define WWAN_TOFF	15

void variant_smi_sleep(u8 slp_typ)
{
	if (slp_typ == ACPI_S5) {
		/* WWAN RW101R-GL power off sequence */
		gpio_set(WWAN_RSL_L, 0);
		mdelay(WWAN_TOFF);
		gpio_set(WWAN_FCPO_L, 0);
	}
}
