/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <fsp/util.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <drivers/intel/gma/opregion.h>
#include <intelblocks/graphics.h>
#include <types.h>

uintptr_t fsp_soc_get_igd_bar(void)
{
	return graphics_get_memory_base();
}

void graphics_soc_init(struct device *dev)
{
	intel_gma_init_igd_opregion();

	/*
	 * GFX PEIM module inside FSP binary is taking care of graphics
	 * initialization based on RUN_FSP_GOP Kconfig
	 * option and input VBT file. Hence no need to load/execute legacy VGA
	 * OpROM in order to initialize GFX.
	 *
	 * In case of non-FSP solution, SoC need to select VGA_ROM_RUN
	 * Kconfig to perform GFX initialization through VGA OpRom.
	 */
	if (CONFIG(RUN_FSP_GOP))
		return;

	/* IGD needs to Bus Master */
	uint32_t reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY | PCI_COMMAND_IO;
	pci_write_config32(dev, PCI_COMMAND, reg32);

	/* Initialize PCI device, load/execute BIOS Option ROM */
	pci_dev_init(dev);
}
