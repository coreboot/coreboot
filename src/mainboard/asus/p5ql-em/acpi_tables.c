/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <soc/nvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	gnvs->pwrs = 1;    /* Power state (AC = 1) */
	gnvs->osys = 2002; /* At least WINXP SP2 (HPET fix) */
	gnvs->cmap = 0x01; /* Enable COM 1 port */
}
