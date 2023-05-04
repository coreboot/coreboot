/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/pci.h>
#include <drivers/intel/gma/libgfxinit.h>
#include <intelblocks/early_graphics.h>
#include <soc/gpio.h>
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

__weak const struct pad_config *variant_early_graphics_gpio_table(size_t *num)
{
	*num = 0;
	return NULL;
}

bool early_graphics_init(void)
{
	int ret;
	const struct pad_config *pads;
	size_t pads_num;

	if (!CONFIG(MAINBOARD_USE_EARLY_LIBGFXINIT))
		return false;

	/* Perform minimal graphic MMIO configuration. */
	device_init();

	/* Optionally configure any required display related GPIOs */
	pads = variant_early_graphics_gpio_table(&pads_num);
	gpio_configure_pads(pads, pads_num);

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
