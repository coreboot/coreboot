/* SPDX-License-Identifier: GPL-2.0-only */

#include <southbridge/intel/i82801gx/nvs.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	gnvs->pwrs = 1; /* Power state (AC = 1) */
	gnvs->cmap = 1; /* COM 1 port */
	gnvs->cmap = 1; /* COM 2 port */
	gnvs->lptp = 0; /* LPT port */
	gnvs->fdcp = 0; /* Floppy Disk Controller */
}
