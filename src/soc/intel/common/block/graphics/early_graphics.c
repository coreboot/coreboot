/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <intelblocks/early_graphics.h>
#include <soc/pci_devs.h>

static void device_init(void)
{
	/* Disable response in IO and MMIO space. */
	pci_and_config16(SA_DEV_IGD, PCI_COMMAND,
			~(PCI_COMMAND_IO | PCI_COMMAND_MEMORY));

	/* Program IGD Base Address Register 0. */
	pci_write_config32(SA_DEV_IGD, PCI_BASE_ADDRESS_0,
			   CONFIG_GFX_GMA_DEFAULT_MMIO);

	/* Enable response in IO and MMIO space. */
	pci_or_config16(SA_DEV_IGD, PCI_COMMAND,
			(PCI_COMMAND_IO | PCI_COMMAND_MEMORY));
}

bool early_graphics_init(void)
{
	int ret;

	if (!CONFIG(MAINBOARD_USE_EARLY_LIBGFXINIT))
		return false;

	/* Perform minimal graphic MMIO configuration. */
	device_init();

	/* Configure display panel. */
	early_graphics_soc_panel_init();

	gma_gfxinit(&ret);
	return !!ret;
}

void early_graphics_stop(void)
{
	if (!CONFIG(MAINBOARD_USE_EARLY_LIBGFXINIT))
		return;

	gma_gfxstop();
}
