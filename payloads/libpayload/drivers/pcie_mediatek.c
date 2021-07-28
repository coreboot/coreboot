/* SPDX-License-Identifier: GPL-2.0-only */

#include <libpayload.h>
#include <pci.h>

#define PCIE_CFGNUM_REG			0x140
#define PCIE_CFG_DEVFN(devfn)		((devfn) & GENMASK(7, 0))
#define PCIE_CFG_BUS(bus)		(((bus) << 8) & GENMASK(15, 8))
#define PCIE_CFG_OFFSET_ADDR		0x1000
#define PCIE_CFG_HEADER(bus, devfn) \
	(PCIE_CFG_BUS(bus) | PCIE_CFG_DEVFN(devfn))

uintptr_t pci_map_bus(pcidev_t dev)
{
	u32 devfn = (PCI_SLOT(dev) << 3) | PCI_FUNC(dev);
	u32 val = PCIE_CFG_HEADER(PCI_BUS(dev), devfn);
	write32((void *)(lib_sysinfo.pcie_ctrl_base + PCIE_CFGNUM_REG), val);

	return lib_sysinfo.pcie_ctrl_base + PCIE_CFG_OFFSET_ADDR;
}
