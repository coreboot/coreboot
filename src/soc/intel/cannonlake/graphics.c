/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016-2017 Intel Corp.
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
#include <drivers/intel/gma/i915_reg.h>
#include <drivers/intel/gma/opregion.h>
#include <intelblocks/graphics.h>

uintptr_t fsp_soc_get_igd_bar(void)
{
	return graphics_get_memory_base();
}

void graphics_soc_init(struct device *dev)
{
	uint32_t ddi_buf_ctl;

	/*
	 * Enable DDI-A (eDP) 4-lane operation if the link is not up yet.
	 * This will allow the kernel to use 4-lane eDP links properly
	 * if the VBIOS or GOP driver do not execute.
	 */
	ddi_buf_ctl = graphics_gtt_read(DDI_BUF_CTL_A);
	if (!acpi_is_wakeup_s3() && !(ddi_buf_ctl & DDI_BUF_CTL_ENABLE)) {
		ddi_buf_ctl |= (DDI_A_4_LANES | DDI_INIT_DISPLAY_DETECTED |
				DDI_BUF_IS_IDLE);
		graphics_gtt_write(DDI_BUF_CTL_A, ddi_buf_ctl);
	}

	/*
	 * GFX PEIM module inside FSP binary is taking care of graphics
	 * initialization based on INTEL_GMA_ADD_VBT Kconfig
	 * option and input VBT file. Hence no need to load/execute legacy VGA
	 * OpROM in order to initialize GFX.
	 *
	 * In case of non-FSP solution, SoC need to select VGA_ROM_RUN
	 * Kconfig to perform GFX initialization through VGA OpRom.
	 */
	if (IS_ENABLED(CONFIG_INTEL_GMA_ADD_VBT))
		return;

	/* IGD needs to Bus Master */
	uint32_t reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Initialize PCI device, load/execute BIOS Option ROM */
	pci_dev_init(dev);
}

uintptr_t graphics_soc_write_acpi_opregion(struct device *device,
		uintptr_t current, struct acpi_rsdp *rsdp)
{
	igd_opregion_t *opregion;

	printk(BIOS_DEBUG, "ACPI:    * IGD OpRegion\n");
	opregion = (igd_opregion_t *)current;

	if (intel_gma_init_igd_opregion(opregion) != CB_SUCCESS)
		return current;

	current += sizeof(igd_opregion_t);

	return acpi_align_current(current);
}
