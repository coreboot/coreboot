/*
 * This file is part of the coreboot project.
 *
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

#include <stdint.h>
#include <arch/acpi.h>
#include <bootmode.h>
#include <console/console.h>
#include <fsp/util.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <intelblocks/graphics.h>
#include <drivers/intel/gma/opregion.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <types.h>

uintptr_t fsp_soc_get_igd_bar(void)
{
	return graphics_get_memory_base();
}

void graphics_soc_init(struct device *const dev)
{
	if (CONFIG(RUN_FSP_GOP))
		return;

	uint32_t reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	if (CONFIG(MAINBOARD_USE_LIBGFXINIT)) {
		if (!acpi_is_wakeup_s3() && display_init_required()) {
			int lightup_ok;
			gma_gfxinit(&lightup_ok);
			gfx_set_init_done(lightup_ok);
		}
	} else {
		/* Initialize PCI device, load/execute BIOS Option ROM */
		pci_dev_init(dev);
	}
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
