/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <types.h>
#include <string.h>
#include <cbmem.h>
#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <arch/acpigen.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <southbridge/intel/fsp_bd82x6x/nvs.h>

#include "thermal.h"

static global_nvs_t *gnvs_;

static void acpi_update_thermal_table(global_nvs_t *gnvs)
{
	gnvs->tcrt = CRITICAL_TEMPERATURE;
	gnvs->tpsv = PASSIVE_TEMPERATURE;
	gnvs->tmax = MAX_TEMPERATURE;
	gnvs->flvl = 5;
}

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	gnvs_ = gnvs;
	memset((void *)gnvs, 0, sizeof(*gnvs));
	gnvs->apic = 1;
	gnvs->mpen = 1; /* Enable Multi Processing */
	gnvs->pcnt = dev_count_cpu();

	/* Enable USB ports in S3 */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;

	/*
	 * Enable Front USB ports in S5 by default
	 * to be consistent with back port behavior
	 */
	gnvs->s5u0 = 1;
	gnvs->s5u1 = 1;

	acpi_update_thermal_table(gnvs);
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				2, IO_APIC_ADDR, 0);

	/* INT_SRC_OVR */
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		 current, 0, 0, 2, 0);
	current += acpi_create_madt_irqoverride((acpi_madt_irqoverride_t *)
		 current, 0, 9, 9, MP_IRQ_TRIGGER_LEVEL | MP_IRQ_POLARITY_HIGH);

	return current;
}
