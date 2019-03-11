/*
 * This file is part of the coreboot project.
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

#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ops.h>

#if (CONFIG_MMCONF_BASE_ADDRESS == 0)
#error "CONFIG_MMCONF_BASE_ADDRESS needs to be non-zero!"
#endif

/*
 * Functions for accessing PCI configuration space with mmconf accesses
 */

#define PCI_MMIO_ADDR(dev, reg, mask)	\
			((void *)(((uintptr_t)CONFIG_MMCONF_BASE_ADDRESS |\
				   (((dev)->bus->secondary & 0xFFF) << 20) |\
				   (((dev)->path.pci.devfn & 0xFF) << 12) |\
				   ((reg) & 0xFFF)) & ~mask))

static uint8_t pci_mmconf_read_config8(const struct device *dev, uint16_t reg)
{
	return read8(PCI_MMIO_ADDR(dev, reg, 0));
}

static uint16_t pci_mmconf_read_config16(const struct device *dev, uint16_t reg)
{
	return read16(PCI_MMIO_ADDR(dev, reg, 1));
}

static uint32_t pci_mmconf_read_config32(const struct device *dev, uint16_t reg)
{
	return read32(PCI_MMIO_ADDR(dev, reg, 3));
}

static void pci_mmconf_write_config8(const struct device *dev, uint16_t reg,
				     uint8_t value)
{
	write8(PCI_MMIO_ADDR(dev, reg, 0), value);
}

static void pci_mmconf_write_config16(const struct device *dev, uint16_t reg,
				      uint16_t value)
{
	write16(PCI_MMIO_ADDR(dev, reg, 1), value);
}

static void pci_mmconf_write_config32(const struct device *dev, uint16_t reg,
				      uint32_t value)
{
	write32(PCI_MMIO_ADDR(dev, reg, 3), value);
}

static const struct pci_bus_operations pci_ops_mmconf = {
	.read8 = pci_mmconf_read_config8,
	.read16 = pci_mmconf_read_config16,
	.read32 = pci_mmconf_read_config32,
	.write8 = pci_mmconf_write_config8,
	.write16 = pci_mmconf_write_config16,
	.write32 = pci_mmconf_write_config32,
};

const struct pci_bus_operations *pci_bus_default_ops(void)
{
	return &pci_ops_mmconf;
}
