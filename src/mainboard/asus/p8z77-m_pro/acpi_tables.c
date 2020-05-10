/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/bd82x6x/nvs.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	/* critical temp that will shutdown the pc == 95C degrees */
	gnvs->tcrt = 95;

	/* temp to start throttling the cpu == 85C */
	gnvs->tpsv = 85;
}
