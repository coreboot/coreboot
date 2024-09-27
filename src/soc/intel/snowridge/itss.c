/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <device/pci_type.h>
#include <intelblocks/itss.h>
#include <soc/itss.h>
#include <soc/pci_devs.h>
#include <static.h>

#include "ramstage.h"

uint32_t itss_soc_get_on_chip_dev_pir(struct device *dev)
{
	/* Check if device is on chip. */
	if (dev->upstream->dev->path.type != DEVICE_PATH_DOMAIN)
		return 0;

	if (!is_pci(dev))
		return 0;

	const unsigned int devfn = dev->path.pci.devfn;
	switch (dev_get_domain_id(dev)) {
	case 0:
		return PCI_ITSS_PIR(PCI_SLOT(devfn));
	case 3:
		if (devfn == NIS_DEVFN)
			return PCI_ITSS_PIR(25);
		break;
	case 4:
		if (devfn == QAT_1_8_DEVFN)
			return PCI_ITSS_PIR(5);
		break;
	}

	printk(BIOS_WARNING, "Unknown PIR for %s\n", dev_path(dev));
	return 0;
}

enum pirq itss_soc_get_dev_pirq(struct device *dev)
{
	enum pci_pin dev_pin = pci_read_config8(dev, PCI_INTERRUPT_PIN);
	if (dev_pin < PCI_INT_A || dev_pin > PCI_INT_D)
		return PIRQ_INVALID;

	struct device *bridge;
	enum pci_pin swizzle_pin;
	if (is_dev_on_domain0(dev)) {
		/**
		 * Get the pin asserting from bridge to root.
		 * This function only applies for devices in domain 0.
		 */
		swizzle_pin = get_pci_irq_pins(dev, &bridge);
		if (swizzle_pin < PCI_INT_A || swizzle_pin > PCI_INT_D)
			return PIRQ_INVALID;

		/* Device attached to PCH PCIe Root Port need further swizzle. */
		if (is_pci_bridge(bridge) && bridge != dev)
			swizzle_pin = pciexp_get_rp_irq_pin(bridge, swizzle_pin);
	} else if (dev->upstream->dev == DEV_PTR(nis_vrp)) {
		bridge = DEV_PTR(nis_vrp);
		swizzle_pin = (dev_pin - 1 + PCI_SLOT(dev->path.pci.devfn)) % 4 + 1;
	} else if (dev->upstream->dev == DEV_PTR(qat_1_8_vrp)) {
		bridge = DEV_PTR(qat_1_8_vrp);
		swizzle_pin = (dev_pin - 1 + PCI_SLOT(dev->path.pci.devfn)) % 4 + 1;
	} else {
		bridge = dev;
		swizzle_pin = dev_pin;
	}

	if (bridge != dev) {
		/**
		 * We can't print `dev` path and `bridge` path in a single `printk()` due to
		 * restriction of `dev_path()`.
		 */
		printk(BIOS_DEBUG, "%s %s swizzled to ", dev_path(dev), pin_to_str(dev_pin));
		printk(BIOS_DEBUG, "%s %s.\n", dev_path(bridge), pin_to_str(swizzle_pin));
	}

	return itss_get_on_chip_dev_pirq(bridge, swizzle_pin);
}
