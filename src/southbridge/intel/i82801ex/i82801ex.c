#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801ex.h"

void i82801ex_enable(device_t dev)
{
	device_t lpc_dev;
	unsigned index = 0;
	uint16_t reg_old, reg;

	/* See if we are behind the i82801ex pci bridge */
	lpc_dev = dev_find_slot(dev->bus->secondary, PCI_DEVFN(0x1f, 0));
	if((dev->path.pci.devfn &0xf8)== 0xf8) {
		index = dev->path.pci.devfn & 7;
	}
	else if((dev->path.pci.devfn &0xf8)== 0xe8) {
		index = (dev->path.pci.devfn & 7) +8;
	}
	if ((!lpc_dev) || (index >= 16) || ((1<<index)&0x3091)) {
		return;
	}
	if ((lpc_dev->vendor != PCI_VENDOR_ID_INTEL) ||
		(lpc_dev->device != PCI_DEVICE_ID_INTEL_82801ER_LPC)) {
		uint32_t id;
		id = pci_read_config32(lpc_dev, PCI_VENDOR_ID);
		if (id != (PCI_VENDOR_ID_INTEL |
				(PCI_DEVICE_ID_INTEL_82801ER_LPC << 16))) {
			return;
		}
	}

	reg = reg_old = pci_read_config16(lpc_dev, 0xf2);
	reg &= ~(1 << index);
	if (!dev->enabled) {
		reg |= (1 << index);
	}
	if (reg != reg_old) {
		pci_write_config16(lpc_dev, 0xf2, reg);
	}

}

struct chip_operations southbridge_intel_i82801ex_ops = {
	CHIP_NAME("Intel ICH5 (82801Ex) Series Southbridge")
	.enable_dev = i82801ex_enable,
};
