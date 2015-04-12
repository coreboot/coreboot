/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Rudolf Marek <r.marek@assembler.cz>
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

#ifndef AMDK8_ACPI_H
#define AMDK8_ACPI_H
#include <arch/acpigen.h>

void k8acpi_write_vars(device_t device);
unsigned long northbridge_write_acpi_tables(device_t device, unsigned long start, acpi_rsdp_t *rsdp);

#endif
