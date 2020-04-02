/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <southbridge/intel/i82801gx/nvs.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	gnvs->pwrs = 1;    /* Power state (AC = 1) */
	gnvs->lptp = 0;    /* LPT port */
	gnvs->fdcp = 0;    /* Floppy Disk Controller */
	gnvs->osys = 2002; /* At least WINXP SP2 (HPET fix) */
	gnvs->apic = 1;    /* Enable APIC */
	gnvs->mpen = 1;    /* Enable Multi Processing */
	gnvs->cmap = 0x01; /* Enable COM 1 port */
}
