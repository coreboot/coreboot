/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <stdint.h>

#include "i82801dx.h"

void i82801dx_enable(struct device *dev)
{
	unsigned int index = 0;
	uint8_t bHasDisableBit = 0;
	uint16_t cur_disable_mask, new_disable_mask;

//      all 82801dbm devices are in bus 0
	unsigned int devfn = PCI_DEVFN(0x1f, 0);	// lpc
	struct device *lpc_dev = pcidev_path_on_root(devfn);	// 0
	if (!lpc_dev)
		return;

	// Calculate disable bit position for specified device:function
	// NOTE: For ICH-4, only the following devices can be disabled:
	//               D31: F0, F1, F3, F5, F6,
	//               D29: F0, F1, F2, F7

	if (PCI_SLOT(dev->path.pci.devfn) == 31) {
		index = PCI_FUNC(dev->path.pci.devfn);

		switch (index) {
		case 0:
		case 1:
		case 3:
		case 5:
		case 6:
			bHasDisableBit = 1;
			break;

		default:
			break;
		};

		if (index == 0)
			index = 14;	// D31:F0 bit is an exception

	} else if (PCI_SLOT(dev->path.pci.devfn) == 29) {
		index = 8 + PCI_FUNC(dev->path.pci.devfn);

		if ((PCI_FUNC(dev->path.pci.devfn) < 3)
		    || (PCI_FUNC(dev->path.pci.devfn) == 7))
			bHasDisableBit = 1;
	}

	if (bHasDisableBit) {
		cur_disable_mask = pci_read_config16(lpc_dev, FUNC_DIS);
		new_disable_mask = cur_disable_mask & ~(1 << index);	// enable it
		if (!dev->enabled) {
			new_disable_mask |= (1 << index);	// disable it
		}
		if (new_disable_mask != cur_disable_mask) {
			pci_write_config16(lpc_dev, FUNC_DIS, new_disable_mask);
		}
	}
}

struct chip_operations southbridge_intel_i82801dx_ops = {
	CHIP_NAME("Intel ICH4/ICH4-M (82801Dx) Series Southbridge")
	    .enable_dev = i82801dx_enable,
};
