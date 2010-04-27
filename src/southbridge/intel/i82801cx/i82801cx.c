#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <assert.h>
#include "i82801cx.h"

void i82801cx_enable(device_t dev)
{
	unsigned int index = 0;
	uint8_t bHasDisableBit = 0;
	uint16_t cur_disable_mask, new_disable_mask;

//	all 82801ca devices are in bus 0
	unsigned int devfn = PCI_DEVFN(0x1f, 0); // lpc
	device_t lpc_dev = dev_find_slot(0, devfn); // 0
	if (!lpc_dev)
		return;

	// Calculate disable bit position for specified device:function
	// NOTE: For ICH-3, only the following devices can be disabled:
	//		 D31:F1, D31:F3, D31:F5, D31:F6,
	//		 D29:F0, D29:F1, D29:F2

    if (PCI_SLOT(dev->path.pci.devfn) == 31) {
    	index = PCI_FUNC(dev->path.pci.devfn);

		if ((index == 1) || (index == 3) || (index == 5) || (index == 6))
			bHasDisableBit = 1;

    } else if (PCI_SLOT(dev->path.pci.devfn) == 29) {
    	index = 8 + PCI_FUNC(dev->path.pci.devfn);

		if (PCI_FUNC(dev->path.pci.devfn) < 3)
			bHasDisableBit = 1;
    }

	if (bHasDisableBit) {
		cur_disable_mask = pci_read_config16(lpc_dev, FUNC_DIS);
		new_disable_mask = cur_disable_mask & ~(1<<index); 		// enable it
		if (!dev->enabled) {
			new_disable_mask |= (1<<index);  // disable it
		}
		if (new_disable_mask != cur_disable_mask) {
			pci_write_config16(lpc_dev, FUNC_DIS, new_disable_mask);
		}
	}
}

struct chip_operations southbridge_intel_i82801cx_ops = {
	CHIP_NAME("Intel ICH3 (82801Cx) Series Southbridge")
	.enable_dev = i82801cx_enable,
};
