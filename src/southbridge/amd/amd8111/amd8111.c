#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "amd8111.h"

void amd8111_enable(device_t dev)
{
	device_t lpc_dev;
	device_t bus_dev;
	unsigned index;
	uint32_t dword;
	uint16_t reg_old, reg;
	uint8_t byte;


	/* See if we are on the behind the amd8111 pci bridge */
	bus_dev = dev->bus->dev;
	if ((bus_dev->vendor == PCI_VENDOR_ID_AMD) && 
	    (bus_dev->device == PCI_DEVICE_ID_AMD_8111_PCI)) {
		unsigned devfn;
		devfn = bus_dev->path.u.pci.devfn + (1 << 3);
		lpc_dev = dev_find_slot(bus_dev->bus->secondary, devfn);
		index = ((dev->path.u.pci.devfn & ~7) >> 3) + 8;
	} else {
		unsigned devfn;
		devfn = (dev->path.u.pci.devfn) & ~7;
		lpc_dev = dev_find_slot(dev->bus->secondary, devfn);
		index = dev->path.u.pci.devfn & 7;
	}

	if ((!lpc_dev) || (index >= 16)) {
		return;
	}
	if ((lpc_dev->vendor != PCI_VENDOR_ID_AMD) ||
	    (lpc_dev->device != PCI_DEVICE_ID_AMD_8111_ISA)) {
		uint32_t id;
		id = pci_read_config32(lpc_dev, PCI_VENDOR_ID);
		if (id != (PCI_VENDOR_ID_AMD | (PCI_DEVICE_ID_AMD_8111_ISA << 16))) {
			return;
		}
	}

        /* Now read the vendor and device id */
        dword= pci_read_config32(dev, PCI_VENDOR_ID);
#if 0
	printk_debug(" %s dev->vendor= %04x, dev->device= %04x, id = %08x\n", dev_path(dev), dev->vendor, dev->device, dword);
#endif
	
        if (dword == (PCI_VENDOR_ID_AMD | (PCI_DEVICE_ID_AMD_8111_USB2 << 16)))  {
		if(!dev->enabled) {
		        byte = pci_read_config8(lpc_dev, 0x47);
		        byte |= (1<<7);
		        pci_write_config8(lpc_dev, 0x47, byte);
			return;
		}
		
	}
	
	reg = reg_old = pci_read_config16(lpc_dev, 0x48);
	reg &= ~(1 << index);
	if (dev->enabled) {
		reg |= (1 << index);
	}
	if (reg != reg_old) {
		pci_write_config16(lpc_dev, 0x48, reg);
	}
	
}

struct chip_operations southbridge_amd_amd8111_ops = {
	.name       = "AMD 8111 Southbridge",
	.enable_dev = amd8111_enable,
};
