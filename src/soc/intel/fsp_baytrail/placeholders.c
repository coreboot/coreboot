/*
 * This file is part of the coreboot project.
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
 */

#include <arch/acpi.h>
#include <device/pci_rom.h>
#include <soc/acpi.h>


void acpi_create_serialio_ssdt(acpi_header_t *ssdt) {}

/* Rmodules don't like weak symbols. */
u32 map_oprom_vendev(u32 vendev) { return vendev; }
