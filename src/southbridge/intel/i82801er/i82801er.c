#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801er.h"

void i82801er_enable(device_t dev)
{
	device_t lpc_dev;
	unsigned int index;
	uint16_t reg_old, reg;

//	all 82801er device ares in bus 0
	unsigned int devfn;
	devfn = PCI_DEVFN(0x1f, 0); // lpc
	lpc_dev = dev_find_slot(0, devfn); // 0
	if (!lpc_dev ) {
		return;
	}
#if 0
	if ((lpc_dev->vendor != PCI_VENDOR_ID_INTEL) ||
	    (lpc_dev->device != PCI_DEVICE_ID_INTEL_82801ER_1F0)) {
		uint32_t id;
		id = pci_read_config32(lpc_dev, PCI_VENDOR_ID);
		if (id != (PCI_VENDOR_ID_INTEL | (PCI_DEVICE_ID_INTEL_82801ER_1F0 << 16))) {
			return;
		}
	}
#endif

	index = (dev->path.u.pci.devfn & 7);
        if((dev->path.u.pci.devfn & ~0x7)==devfn) { // D=0x1f
                if(index==0){   //1f0   
                        index = 14;
                } 
        } else { // D=0x1d
                index += 8;
        }

	reg_old = pci_read_config16(lpc_dev, FUNC_DIS);
	reg = reg_old;
	reg &= ~(1<<index); // enable it
	if (!dev->enabled) {
		reg |= (1<<index);  // disable it
	}
	if (reg != reg_old) {
		pci_write_config16(lpc_dev, FUNC_DIS, reg);
	}
	reg = pci_read_config16(lpc_dev, FUNC_DIS);

}

struct chip_operations southbridge_intel_i82801er_control = {
	.name       = "Intel 82801er Southbridge",
	.enable_dev = i82801er_enable,
};
