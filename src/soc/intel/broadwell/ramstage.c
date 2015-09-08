/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <device/device.h>
#include <stdlib.h>
#include <string.h>
#include <soc/nvs.h>
#include <soc/pm.h>
#include <soc/ramstage.h>
#include <soc/intel/broadwell/chip.h>
#include <soc/intel/common/acpi.h>

/* Save wake source information for calculating ACPI _SWS values */
int soc_fill_acpi_wake(uint32_t *pm1, uint32_t **gpe0)
{
	struct chipset_power_state *ps = cbmem_find(CBMEM_ID_POWER_STATE);
	static uint32_t gpe0_sts[GPE0_REG_MAX];
	int i;

	*pm1 = ps->pm1_sts & ps->pm1_en;

	/* Mask off GPE0 status bits that are not enabled */
	*gpe0 = &gpe0_sts[0];
	for (i = 0; i < GPE0_REG_MAX; i++)
		gpe0_sts[i] = ps->gpe0_sts[i] & ps->gpe0_en[i];

	return GPE0_REG_MAX;
}

static void s3_resume_prepare(void)
{
	global_nvs_t *gnvs;

	gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(global_nvs_t));
	if (gnvs == NULL)
		return;

	if (!acpi_is_wakeup_s3())
		memset(gnvs, 0, sizeof(global_nvs_t));
}

void broadwell_init_pre_device(void *chip_info)
{
	s3_resume_prepare();
	broadwell_run_reference_code();
}
