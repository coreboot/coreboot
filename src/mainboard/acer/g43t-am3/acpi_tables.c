/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <soc/nvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	gnvs->osys = 2002; /* At least WINXP SP2 (HPET fix) */
}
