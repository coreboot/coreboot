/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2017 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/acpigen.h>
#include <console/console.h>
#include <fsp/util.h>
#include <device/device.h>
#include <device/pci.h>
#include <intelblocks/graphics.h>
#include <drivers/intel/gma/opregion.h>

uintptr_t fsp_soc_get_igd_bar(void)
{
	return graphics_get_memory_base();
}

uintptr_t graphics_soc_write_acpi_opregion(struct device *device,
		uintptr_t current, struct acpi_rsdp *rsdp)
{
	igd_opregion_t *opregion;

	printk(BIOS_DEBUG, "ACPI:    * IGD OpRegion\n");
	opregion = (igd_opregion_t *)current;

	if (intel_gma_init_igd_opregion(opregion) != CB_SUCCESS)
		return current;

	/* FIXME: Add platform specific mailbox initialization */

	current += sizeof(igd_opregion_t);
	return acpi_align_current(current);
}
