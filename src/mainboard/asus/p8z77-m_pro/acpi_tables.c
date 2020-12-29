/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <soc/nvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* critical temp that will shutdown the pc == 95C degrees */
	gnvs->tcrt = 95;

	/* temp to start throttling the cpu == 85C */
	gnvs->tpsv = 85;
}
