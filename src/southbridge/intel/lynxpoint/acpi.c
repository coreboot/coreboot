/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <string.h>
#include <version.h>

#include "pch.h"
#include "nvs.h"

static void acpi_create_serialio_ssdt_entry(int id, struct global_nvs *gnvs)
{
	char sio_name[5] = {};
	snprintf(sio_name, sizeof(sio_name), "S%1uEN", id);
	acpigen_write_name_byte(sio_name, gnvs->s0b[id] ? 1 : 0);
}

void acpi_create_serialio_ssdt(acpi_header_t *ssdt)
{
	unsigned long current = (unsigned long)ssdt + sizeof(acpi_header_t);
	struct global_nvs *gnvs = acpi_get_gnvs();
	int id;

	if (!gnvs)
		return;

	/* Fill the SSDT header */
	memset((void *)ssdt, 0, sizeof(acpi_header_t));
	memcpy(&ssdt->signature, "SSDT", 4);
	ssdt->revision = get_acpi_table_revision(SSDT);
	memcpy(&ssdt->oem_id, OEM_ID, 6);
	memcpy(&ssdt->oem_table_id, "SERIALIO", 8);
	ssdt->oem_revision = 43;
	memcpy(&ssdt->asl_compiler_id, ASLC, 4);
	ssdt->asl_compiler_revision = asl_revision;
	ssdt->length = sizeof(acpi_header_t);
	acpigen_set_current((char *)current);

	/* Fill the SSDT with an entry for each SerialIO device */
	for (id = 0; id < 8; id++)
		acpi_create_serialio_ssdt_entry(id, gnvs);

	/* (Re)calculate length and checksum. */
	current = (unsigned long)acpigen_get_current();
	ssdt->length = current - (unsigned long)ssdt;
	ssdt->checksum = acpi_checksum((void *)ssdt, ssdt->length);
}
