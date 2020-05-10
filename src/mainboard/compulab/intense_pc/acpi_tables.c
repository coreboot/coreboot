/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/bd82x6x/nvs.h>

/* FIXME: check this function.  */
void acpi_create_gnvs(global_nvs_t *gnvs)
{
	gnvs->tcrt = 100;
	gnvs->tpsv = 90;
}
