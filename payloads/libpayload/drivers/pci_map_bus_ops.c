/* SPDX-License-Identifier: GPL-2.0-only */

#include <libpayload.h>
#include <pci.h>

u8 pci_read_config8(pcidev_t dev, u16 reg)
{
	uintptr_t cfg_base = pci_map_bus(dev);

	return read8((void *)(cfg_base | reg));
}

u16 pci_read_config16(pcidev_t dev, u16 reg)
{
	uintptr_t cfg_base = pci_map_bus(dev);

	return read16((void *)(cfg_base | (reg & ~1)));
}

u32 pci_read_config32(pcidev_t dev, u16 reg)
{
	uintptr_t cfg_base = pci_map_bus(dev);

	return read32((void *)(cfg_base | (reg & ~3)));
}

void pci_write_config8(pcidev_t dev, u16 reg, u8 val)
{
	uintptr_t cfg_base = pci_map_bus(dev);

	write8((void *)(cfg_base | reg), val);
}

void pci_write_config16(pcidev_t dev, u16 reg, u16 val)
{
	uintptr_t cfg_base = pci_map_bus(dev);

	write16((void *)(cfg_base | (reg & ~1)), val);
}

void pci_write_config32(pcidev_t dev, u16 reg, u32 val)
{
	uintptr_t cfg_base = pci_map_bus(dev);

	write32((void *)(cfg_base | (reg & ~3)), val);
}
