/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2017-present Facebook, Inc.
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

#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <arch/acpigen.h>
#include <arch/smp/mpspec.h>
#include <soc/acpi.h>
#include <soc/nvs.h>
#include <soc/iomap.h>

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	acpi_init_gnvs(gnvs);

	/* Enable USB ports in S3 */
	gnvs->s3u0 = 1;
	gnvs->s3u1 = 1;

	/* Disable USB ports in S5 */
	gnvs->s5u0 = 0;
	gnvs->s5u1 = 0;

	/* TPM Present */
	gnvs->tpmp = 1;

	/* Enable DPTF */
	gnvs->dpte = 0;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	current += acpi_create_madt_ioapic((acpi_madt_ioapic_t *) current,
				2, IO_APIC_ADDR, 0);

	current = acpi_madt_irq_overrides(current);

	return current;
}
