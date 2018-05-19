#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "amd8111.h"

void amd8111_enable(struct device *dev)
{
	struct device *lpc_dev;
	struct device *bus_dev;
	unsigned index;
	unsigned reg_old, reg;

	/* See if we are on the bus behind the amd8111 pci bridge */
	bus_dev = dev->bus->dev;
	if ((bus_dev->vendor == PCI_VENDOR_ID_AMD) &&
	    (bus_dev->device == PCI_DEVICE_ID_AMD_8111_PCI))
	{
		unsigned devfn;
		devfn = bus_dev->path.pci.devfn + (1 << 3);
		lpc_dev = dev_find_slot(bus_dev->bus->secondary, devfn);
		index = ((dev->path.pci.devfn & ~7) >> 3) + 8;
		if (dev->path.pci.devfn == 2) { /* EHCI */
			index = 16;
		}
	} else {
		unsigned devfn;
		devfn = (dev->path.pci.devfn) & ~7;
		lpc_dev = dev_find_slot(dev->bus->secondary, devfn);
		index = dev->path.pci.devfn & 7;
	}
	if ((!lpc_dev) || (index >= 17)) {
		return;
	}
	if ((lpc_dev->vendor != PCI_VENDOR_ID_AMD) ||
	    (lpc_dev->device != PCI_DEVICE_ID_AMD_8111_ISA))
	{
		uint32_t id;
		id = pci_read_config32(lpc_dev, PCI_VENDOR_ID);
		if (id != (PCI_VENDOR_ID_AMD | (PCI_DEVICE_ID_AMD_8111_ISA << 16))) {
			return;
		}
	}

	if (index < 16) {
		reg = reg_old = pci_read_config16(lpc_dev, 0x48);
		reg &= ~(1 << index);
		if (dev->enabled) {
			reg |= (1 << index);
		}
		if (reg != reg_old) {
			pci_write_config16(lpc_dev, 0x48, reg);
		}
	}
	else if (index == 16) {
		reg = reg_old = pci_read_config8(lpc_dev, 0x47);
		reg &= ~(1 << 7);
		if (!dev->enabled) {
			reg |= (1 << 7);
		}
		if (reg != reg_old) {
			pci_write_config8(lpc_dev, 0x47, reg);
		}
	}
}

struct chip_operations southbridge_amd_amd8111_ops = {
	CHIP_NAME("AMD-8111 Southbridge")
	/* This only called when this device is listed in the
	* static device tree.
	*/
	.enable_dev = amd8111_enable,
};
