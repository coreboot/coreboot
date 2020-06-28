/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <southbridge/intel/i82801gx/nvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	gnvs->pwrs = 1; /* Power state (AC = 1) */
	gnvs->cmap = 1; /* COM 1 port */
	gnvs->cmap = 1; /* COM 2 port */
	gnvs->lptp = 0; /* LPT port */
	gnvs->fdcp = 0; /* Floppy Disk Controller */
}
