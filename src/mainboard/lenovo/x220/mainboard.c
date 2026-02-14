/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_def.h>
#include <drivers/intel/gma/int15.h>
#include <option.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <stdio.h>

static void mainboard_enable(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS,
					GMA_INT15_PANEL_FIT_DEFAULT,
					GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

static void mainboard_init(void *chip_info)
{
	char option[20];

	/* Apply user config and disable ports if option exists and is 0 */
	for (struct device *dev = all_devices; dev; dev = dev->next) {
		/* Only care about PCH PCIe root ports */
		if (PCI_SLOT(dev->path.pci.devfn) !=
		    PCH_PCIE_DEV_SLOT)
			continue;
		/*
		 * Only care about ports enabled at this point. Ports that are
		 * disabled here are not implemented on this mainboard.
		 */
		if (!dev->enabled)
			continue;

		const unsigned int idx = PCI_FUNC(dev->path.pci.devfn);
		snprintf(option, sizeof(option), "pch_pcie_enable_rp%u", idx);

		/* When option is not found keep current configuration */
		dev->enabled = get_uint_option(option, dev->enabled);
		printk(BIOS_DEBUG, "%s: %s %sabled\n", __func__, dev_path(dev),
		       dev->enabled ? "en" : "dis");
	}
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
