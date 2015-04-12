/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
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

#include <types.h>
#include <string.h>
#include <console/console.h>
#include <arch/acpi.h>
#include <arch/ioapic.h>
#include <arch/acpigen.h>
#include <arch/smp/mpspec.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>

#include "southbridge/intel/i82801gx/nvs.h"
#include "mainboard.h"

void acpi_create_gnvs(global_nvs_t *gnvs)
{
	/* Enable COM port(s) */
	gnvs->cmap = 0x01;
	gnvs->cmbp = 0x00;
}

static long acpi_create_ecdt(acpi_ecdt_t * ecdt)
{
	/* Attention: Make sure these match the values from
	 * the DSDT's ec.asl
	 */
	static const char ec_id[] = "\\_SB.PCI0.LPCB.EC0";
	int ecdt_len = sizeof(acpi_ecdt_t) + strlen(ec_id) + 1;

	acpi_header_t *header = &(ecdt->header);

	memset((void *) ecdt, 0, ecdt_len);

	/* fill out header fields */
	memcpy(header->signature, "ECDT", 4);
	memcpy(header->oem_id, OEM_ID, 6);
	memcpy(header->oem_table_id, ACPI_TABLE_CREATOR, 8);
	memcpy(header->asl_compiler_id, ASLC, 4);

	header->length = ecdt_len;
	header->revision = 1;

	/* Location of the two EC registers */
	ecdt->ec_control.space_id = ACPI_ADDRESS_SPACE_IO;
	ecdt->ec_control.bit_width = 8;
	ecdt->ec_control.bit_offset = 0;
	ecdt->ec_control.addrl = 0x66;
	ecdt->ec_control.addrh = 0;

	ecdt->ec_data.space_id = ACPI_ADDRESS_SPACE_IO;	/* Memory */
	ecdt->ec_data.bit_width = 8;
	ecdt->ec_data.bit_offset = 0;
	ecdt->ec_data.addrl = 0x62;
	ecdt->ec_data.addrh = 0;

	ecdt->uid = 1; // Must match _UID of the EC0 node.

	ecdt->gpe_bit = 23; // SCI interrupt within GPEx_STS

	strncpy((char *)ecdt->ec_id, ec_id, strlen(ec_id));

	header->checksum =
	    acpi_checksum((void *) ecdt, ecdt_len);

	return header->length;
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

	/* LAPIC_NMI */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
				current, 0, 0x0005, 0x01);
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)
				current, 1, 0x0005, 0x01);

	return current;
}

#define ALIGN_CURRENT current = (ALIGN(current, 16))
unsigned long mainboard_write_acpi_tables(device_t device,
					  unsigned long start,
					  acpi_rsdp_t *rsdp)
{
	unsigned long current;
	acpi_header_t *ecdt;

	current = start;

	/* Align ACPI tables to 16byte */
	ALIGN_CURRENT;

	printk(BIOS_DEBUG, "ACPI:     * ECDT\n");
	ecdt = (acpi_header_t *)current;
	current += acpi_create_ecdt((acpi_ecdt_t *)current);
	ALIGN_CURRENT;
	acpi_add_table(rsdp, ecdt);

	printk(BIOS_DEBUG, "current = %lx\n", current);
	return current;
}
