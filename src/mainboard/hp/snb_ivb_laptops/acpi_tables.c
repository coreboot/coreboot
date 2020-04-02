/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <southbridge/intel/bd82x6x/nvs.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	// the lid is open by default.
	gnvs->lids = 1;

	gnvs->tcrt = 100;
	gnvs->tpsv = 90;
}
