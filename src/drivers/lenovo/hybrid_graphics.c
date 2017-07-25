/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Patrick Rudolph
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <types.h>
#include <string.h>
#include <option.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <device/pci.h>
#include <console/console.h>
#include <southbridge/intel/common/gpio.h>

/* Hybrid graphics allows to connect LVDS interface to either iGPU
 * or dGPU depending on GPIO level.
 * Nvidia is calling this functionality "muxed Optimus".
 * Some devices, like T430s, only support "muxless Optimus" where the
 * Intel GPU is always connected to the panel.
 * As it is only linked on lenovo and only executed if the GPU exists
 * we know for sure that the dGPU is there and connected to first PEG slot.
 *
 * Note: Once native gfx init is done for AMD or Nvida graphic
 *       cards, merge this code.
 */

#define HYBRID_GRAPHICS_INTEGRATED 0
#define HYBRID_GRAPHICS_DISCRETE 1

static void hybrid_graphics_disable_peg(struct device *dev)
{
	struct device *peg_dev;

	/* connect LVDS interface to iGPU */
	set_gpio(CONFIG_HYBRID_GRAPHICS_GPIO_NUM, GPIO_LEVEL_HIGH);
	printk(BIOS_DEBUG, "Hybrid graphics: Switching panel to integrated GPU.\n");
	dev->enabled = 0;

	/* Disable PEG10 */
	peg_dev = dev_find_slot(0, PCI_DEVFN(1, 0));
	if (peg_dev)
		peg_dev->enabled = 0;

	printk(BIOS_DEBUG, "Hybrid graphics: Disabled PEG10.\n");
}

/* Called before VGA enable bits are set and only if dGPU
 * is present. Enable/disable VGA devices here. */
static void hybrid_graphics_enable_peg(struct device *dev)
{
	u8 hybrid_graphics_mode;

	hybrid_graphics_mode = HYBRID_GRAPHICS_INTEGRATED;
	get_option(&hybrid_graphics_mode, "hybrid_graphics_mode");

	if (hybrid_graphics_mode == HYBRID_GRAPHICS_DISCRETE) {
		/* connect LVDS interface to dGPU */
		set_gpio(CONFIG_HYBRID_GRAPHICS_GPIO_NUM, GPIO_LEVEL_LOW);
		printk(BIOS_DEBUG, "Hybrid graphics: Switching panel to discrete GPU.\n");
		dev->enabled = 1;

		/* Disable IGD */
		dev = dev_find_slot(0, PCI_DEVFN(2, 0));
		if (dev && dev->ops->disable)
			dev->ops->disable(dev);
		dev->enabled = 0;

		printk(BIOS_DEBUG, "Hybrid graphics: Disabled IGD.\n");
	} else
		hybrid_graphics_disable_peg(dev);
}

static struct pci_operations pci_dev_ops_pci = {
	.set_subsystem = pci_dev_set_subsystem,
};

struct device_operations hybrid_graphics_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = pci_dev_init,
	.scan_bus         = 0,
	.enable           = hybrid_graphics_enable_peg,
	.disable          = hybrid_graphics_disable_peg,
	.ops_pci          = &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids_nvidia[] = {
		0x0ffc, /* Nvidia NVS Quadro K1000m Lenovo W530 */
		0x0def, /* NVidia NVS 5400m Lenovo T430/T530 */
		0x0dfc, /* NVidia NVS 5200m Lenovo T430s */
		0x1056, /* NVidia NVS 4200m Lenovo T420/T520 */
		0x1057, /* NVidia NVS 4200m Lenovo T420/T520 */
		0x0a6c, /* NVidia NVS 3100m Lenovo T410/T510 */
		0 };

static const struct pci_driver hybrid_peg_nvidia __pci_driver = {
	.ops	 = &hybrid_graphics_ops,
	.vendor	 = PCI_VENDOR_ID_NVIDIA,
	.devices = pci_device_ids_nvidia,
};
