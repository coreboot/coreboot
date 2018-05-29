/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <device/cardbus.h>
#include "pci7420.h"
#include "chip.h"

#ifdef ODD_IRQ_FIXUP
static int cardbus_count = 0;
#endif

static void pci7420_cardbus_init(struct device *dev)
{
	u8 reg8;
	u16 reg16;
	u32 reg32;

	struct southbridge_ti_pci7420_config *config = dev->chip_info;
	int smartcard_enabled = 0;

	printk(BIOS_DEBUG, "TI PCI7420/7620 init\n");

	if (!config) {
		printk(BIOS_DEBUG, "PCI7420: No configuration found.\n");
	} else {
		smartcard_enabled = config->smartcard_enabled;
	}

	reg32 = pci_read_config32(dev, SYSCTL);
	reg32 |= RIMUX;
	pci_write_config32(dev, SYSCTL, reg32);

	/* Enable SPKROUT */
	reg8 = pci_read_config8(dev, CARDCTL);
	reg8 |= SPKROUTEN;
	pci_write_config8(dev, CARDCTL, reg8);

	/* Power switch select and FM disable */
	reg16 = pci_read_config16(dev, GENCTL);
	reg16 |= P12V_SW_SEL; // 12V capable power switch
	if (smartcard_enabled == 0)
		reg16 |= DISABLE_FM;
	pci_write_config16(dev, GENCTL, reg16);

	/* Multifunction routing status */
	pci_write_config32(dev, MFUNC, 0x018a1b22);

#ifdef ODD_IRQ_FIXUP
	/* This is a workaround for buggy kernels. This should
	 * probably be read from the device tree, but as long
	 * as only one mainboard is using this bridge it does
	 * not matter.
	 *
	 * Basically what we do here is assign INTA to the first
	 * cardbus controller, and INTB to the second one. We know
	 * there are only two of them.
	 */
	pci_write_config8(dev, PCI_INTERRUPT_PIN, cardbus_count);
	cardbus_count++;
#endif
}

static void pci7420_cardbus_read_resources(struct device *dev)
{
	cardbus_read_resources(dev);
}

static void pci7420_cardbus_set_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "%s In set resources\n",dev_path(dev));

	pci_dev_set_resources(dev);

	printk(BIOS_DEBUG, "%s done set resources\n",dev_path(dev));
}

static struct device_operations ti_pci7420_ops = {
	.read_resources   = pci7420_cardbus_read_resources,
	.set_resources    = pci7420_cardbus_set_resources,
	.enable_resources = cardbus_enable_resources,
	.init             = pci7420_cardbus_init,
	.scan_bus         = pci_scan_bridge,
};

static const struct pci_driver ti_pci7420_driver __pci_driver = {
	.ops    = &ti_pci7420_ops,
	.vendor = 0x104c,
	.device = 0xac8e,
};

static const struct pci_driver ti_pci7620_driver __pci_driver = {
	.ops    = &ti_pci7420_ops,
	.vendor = 0x104c,
	.device = 0xac8d,
};

static void ti_pci7420_enable_dev(struct device *dev)
{
	/* Nothing here yet */
}

struct chip_operations southbridge_ti_pci7420_ops = {
	CHIP_NAME("Texas Instruments PCI7420/7620 Cardbus Controller")
	.enable_dev    = ti_pci7420_enable_dev,
};
