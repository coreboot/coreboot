/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <soc/nvs.h>

void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	gnvs->pwrs = 1;    /* Power state (AC = 1) */
	gnvs->lptp = 0;    /* LPT port */
	gnvs->fdcp = 0;    /* Floppy Disk Controller */
	gnvs->osys = 2002; /* At least WINXP SP2 (HPET fix) */
	gnvs->apic = 1;    /* Enable APIC */
	gnvs->mpen = 1;    /* Enable Multi Processing */
	gnvs->cmap = 0x01; /* Enable COM 1 port */
}
