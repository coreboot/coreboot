/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/i82801gx/nvs.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	gnvs->pwrs = 1;    /* Power state (AC = 1) */
	gnvs->cmap = 0x01; /* Enable COM 1 port */
}
